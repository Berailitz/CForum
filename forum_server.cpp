#include "forum_server.h"

namespace cforum
{
    /**
     * @brief 创建服务器控制器，连接WebSocket连接相关的槽和信号。
     *
     */
    ForumServer::ForumServer() :
        QWebSocketServer(QStringLiteral("WS Server"),
            QWebSocketServer::NonSecureMode),
        cforum(new CForum()),
        clients(new QVector<ClientDescriptor *>)
    {
        connect(this,
            qOverload<const QString &,
            const QString &>(&ForumServer::messageToSend),
            this,
            qOverload<const QString &,
            const QString &>(&ForumServer::onMessageToSend));
        connect(this, &QWebSocketServer::newConnection,
            this, &ForumServer::onNewConnection);
    }

    /**
     * @brief 析构服务器控制器，断开与客户端的连接。
     *
     */
    ForumServer::~ForumServer()
    {
        delete cforum;
        for (ClientDescriptor * client : *clients)
        {
            client->getSocket()->close();
            client->deleteLater();
        }
        delete clients;
    }

    /**
     * @brief 启动服务器。
     *
     * @param portString
     * @return true
     * @return false
     */
    bool ForumServer::start(const QString portString)
    {
        int port = portString.toInt();
        return start(port);
    }

    /**
     * @brief 启动服务器，如遇异常，报错并返回假。
     *
     * @param port
     * @return true
     * @return false
     */
    bool ForumServer::start(const int port)
    {
        if (isListening())
        {
            emit messageReceived(SERVER_ALREADY_START_MESSAGE + Q_LINE_BREAK);
            return true;
        }
        else
        {
            if (QWebSocketServer::listen(QHostAddress::Any, port))
            {
                QString message = SERVER_START_MESSAGE;
                QList<QHostAddress> addressList = QNetworkInterface::allAddresses();
                for (QList<QHostAddress>::iterator it = addressList.begin();
                    it != addressList.end();
                    it++)
                {
                    {
                        message += (it->toString() + Q_LINE_BREAK);
                    }
                }
                emit messageReceived(message);
                return true;
            }
            else
            {
                emit messageReceived(SERVER_START_ERROR_MESSAGE + Q_LINE_BREAK);
                return false;
            }
        }
    }

    /**
     * @brief 停止服务器。
     *
     */
    void ForumServer::stop()
    {
        close();
        emit messageReceived(SERVER_STOP_MESSAGE + Q_LINE_BREAK);
    }

    bool ForumServer::load(const fs::path path)
    {
        lastPath = path;
        return cforum->load(path);
    }

    bool ForumServer::save() const
    {
        return save(lastPath);
    }

    bool ForumServer::save(const fs::path path) const
    {
        return cforum->save(path);
    }

    void ForumServer::onErrorRaised(const QString message)
    {
        emit messageReceived(message + Q_LINE_BREAK);
    }

    /**
     * @brief 连接客户端。
     *
     */
    void ForumServer::onNewConnection()
    {
        QWebSocket *socket = QWebSocketServer::nextPendingConnection();
        ClientDescriptor *newClient = new ClientDescriptor(socket);
        emit messageReceived(newClient->hash() + SERVER_CONNECTED_MESSAGE + Q_LINE_BREAK);

        connect(socket, &QWebSocket::textMessageReceived,
            this, &ForumServer::onTextMessageReceived);
        connect(socket, &QWebSocket::disconnected,
            this, &ForumServer::onDisconnection);
        *clients << newClient;
    }

    /**
     * @brief 创建新线程，并处理客户端的请求。
     *
     * @param textMessage
     */
    void ForumServer::onTextMessageReceived(const QString &textMessage)
    {
        QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
        RequestMessage message(textMessage);
        QString hash = hashSocket(*socket);
        emit messageReceived(hash + QString::fromUtf8(" > ") + textMessage + Q_LINE_BREAK);
        QtConcurrent::run(this, &ForumServer::execute, hashSocket(*socket), message);
    }

    /**
     * @brief 断开与客户端的连接，更新连接池、客户端池，完成善后工作。
     *
     */
    void ForumServer::onDisconnection()
    {
        QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
        QString hash = hashSocket(*socket);
        for (auto *cit = clients->begin(); cit != clients->end(); cit++)
        {
            if ((*cit)->hash() == hash)
            {
                clients->erase(cit);
                break;
            }
        }
        emit messageReceived(hash + SERVER_DISCONNECTED_MESSAGE + Q_LINE_BREAK);
    }

    /**
     * @brief 从sockst发送消息的唯一方法。
     *
     * @param socket
     * @param textMessage
     */
    void ForumServer::sendMessage(QWebSocket & socket, const QString & textMessage)
    {
        QString hash = hashSocket(socket);
        socket.sendTextMessage(textMessage);
        emit messageReceived(hash + " < " + textMessage + Q_LINE_BREAK);
    }

    /**
     * @brief 从客户端唯一标识hash发送消息的唯一方法。
     *
     * @param target
     * @param textMessage
     */
    void ForumServer::onMessageToSend(const QString & target, const QString & textMessage)
    {
        for (ClientDescriptor *client : *clients)
        {
            if (client->hash() == target)
            {
                sendMessage(*client->getSocket(), textMessage);
            }
        }
    }

    /**
     * @brief 执行客户端的请求，该方法在新线程中执行，使用信号-槽机制异步回调。
     * 采用有序资源使用算法避免死锁。
     *
     * @param target
     * @param request
     */
    void ForumServer::execute(const QString &target, const RequestMessage & request)
    {
        QString messageString = request.getMessageString();
        istringstream iss(messageString.toStdString());
        int boardID, postID, commentID, userID;
        string boardName;
        string userName;
        string password;
        switch (request.getType())
        {
        case GetBoardListRequestMessageType:
            sendBoardList(target);
            break;
        case GetPostListRequestMessageType:
            iss >> boardID;
            sendPostList(target, boardID);
            break;
        case GetCommentListRequestMessageType:
            iss >> boardID;
            iss >> postID;
            sendCommentList(target, boardID, postID);
            break;
        case RegisterRequestMessageType:
            getline(iss, userName);
            getline(iss, password);
            addNormalUser(target,
                QString::fromStdString(userName),
                QString::fromStdString(password));
            break;
        case LoginRequestMessageType:
            getline(iss, userName);
            getline(iss, password);
            login(target,
                QString::fromStdString(userName),
                QString::fromStdString(password));
            break;
        case AddBoardMessageType:
            getline(iss, boardName);
            addBoard(target, QString::fromStdString(boardName));
            break;
        case AddPostMessageType:
            iss >> boardID;
            iss.get();
            addPost(target, boardID, iss);
            break;
        case RemovePostMessageType:
            iss >> boardID;
            iss >> postID;
            iss >> userID;
            removePost(target, boardID, postID, userID);
            break;
        case AddCommentMessageType:
            iss >> boardID;
            iss >> postID;
            iss.get();
            addComment(target, boardID, postID, iss);
            break;
        case RemoveCommentMessageType:
            iss >> boardID;
            iss >> postID;
            iss >> commentID;
            iss >> userID;
            removeComment(target, boardID, postID, commentID, userID);
            break;
        default:
            break;
        }

        QMutexLocker userLocker(&userMutex);
        QMutexLocker contentLocker(&contentMutex);
        QMutexLocker fileLocker(&fileMutex);
        save();
    }

    /**
     * @brief 将一个版面的信息发送给一个客户端。
     *
     * @param target
     * @param board
     */
    void ForumServer::sendBoard(const QString &target, const Board & board)
    {
        ostringstream oss;
        oss << AddBoardMessageType << STD_LINE_BREAK << board;
        emit messageToSend(target, QString::fromStdString(oss.str()));
    }

    /**
     * @brief 将所有版面的信息发送给一个客户端。
     *
     * @param target
     */
    void ForumServer::sendBoardList(const QString &target)
    {
        QMutexLocker contentLocker(&contentMutex);
        for (QObject *&qit : *cforum->getBoards())
        {
            sendBoard(target, *static_cast<Board*>(qit));
        }
    }

    /**
     * @brief 将一个版面的信息发送给所有客户端。
     *
     * @param board
     */
    void ForumServer::broadcastBoard(const Board & board)
    {
        ClientHashList *hashList = getAllClientHash();
        for (QString hash : *hashList)
        {
            sendBoard(hash, board);
        }
        delete hashList;
    }

    /**
     * @brief 将一个主题帖的信息发送给一个客户端。
     *
     * @param target
     * @param boardID
     * @param post
     */
    void ForumServer::sendPost(const QString &target, const int boardID, const Post &post)
    {
        ostringstream oss;
        oss << AddPostMessageType << STD_LINE_BREAK << boardID << STD_LINE_BREAK << post;
        emit messageToSend(target, QString::fromStdString(oss.str()));
    }

    /**
     * @brief 将所有主题帖的信息发送给一个客户端。
     *
     * @param target
     * @param boardID
     */
    void ForumServer::sendPostList(const QString &target, const int boardID)
    {
        Board *board;
        QMutexLocker contentLocker(&contentMutex);
        board = cforum->getBoardByID(boardID);
        if (board)
        {
            for (QObject *&qit : *board->getPosts())
            {
                sendPost(target, boardID, *static_cast<Post*>(qit));
            }
        }
    }

    /**
     * @brief 将一个主题帖的信息发送给所有客户端。
     *
     * @param boardID
     * @param post
     */
    void ForumServer::broadcastPost(const int boardID, const Post & post)
    {
        ClientHashList *hashList = getAllClientHash();
        for (QString hash : *hashList)
        {
            sendPost(hash, boardID, post);
        }
        delete hashList;
    }

    /**
     * @brief 将一个主题帖的删除信息发送给所有客户端。
     *
     * @param boardID
     * @param postID
     */
    void ForumServer::broadcastPostDeletion(const int boardID, const int postID)
    {
        ClientHashList *hashList = getAllClientHash();
        RequestMessage message;
        message.removePost(boardID, postID, -1);
        for (QString hash : *hashList)
        {
            emit messageToSend(hash, message.dump());
        }
        delete hashList;
    }

    /**
     * @brief 将一个回复帖的信息发送给一个客户端。
     *
     * @param target
     * @param boardID
     * @param postID
     * @param comment
     */
    void ForumServer::sendComment(const QString &target,
        const int boardID,
        const int postID,
        const Comment &comment)
    {
        ostringstream oss;
        oss << AddCommentMessageType << STD_LINE_BREAK << boardID;
        oss << STD_LINE_BREAK << postID << STD_LINE_BREAK << comment;
        emit messageToSend(target, QString::fromStdString(oss.str()));
    }

    /**
     * @brief 将所有回复帖的信息发送给一个客户端。
     *
     * @param target
     * @param boardID
     * @param postID
     */
    void ForumServer::sendCommentList(const QString &target,
        const int boardID,
        const int postID)
    {
        Board *board;
        Post *post;
        QMutexLocker contentLocker(&contentMutex);
        board = cforum->getBoardByID(boardID);
        if (board)
        {
            post = board->getPostByID(postID);
            if (post)
            {
                for (QObject *&qit : *post->getComments())
                {
                    sendComment(target, boardID, postID, *static_cast<Comment *>(qit));
                }
            }
        }
    }

    /**
     * @brief 将一个回复帖的信息发送给所有客户端。
     *
     * @param boardID
     * @param postID
     * @param comment
     */
    void ForumServer::broadcastComment(const int boardID,
        const int postID,
        const Comment &comment)
    {
        ClientHashList *hashList = getAllClientHash();
        for (QString hash : *hashList)
        {
            sendComment(hash, boardID, postID, comment);
        }
        delete hashList;
    }

    /**
     * @brief 将一个回复帖的删除信息发送给所有客户端。
     *
     * @param boardID
     * @param postID
     * @param commentID
     */
    void ForumServer::broadcastCommentDeletion(const int boardID,
        const int postID,
        const int commentID)
    {
        ClientHashList *hashList = getAllClientHash();
        RequestMessage message;
        message.removeComment(boardID, postID, commentID, -1);
        for (QString hash : *hashList)
        {
            emit messageToSend(hash, message.dump());
        }
        delete hashList;
    }

    /**
     * @brief 在一个客户端上显示提示消息。
     *
     * @param target
     * @param text
     */
    void ForumServer::sendToast(const QString &target, const QString &text)
    {
        ostringstream oss;
        oss << ToastResponseMessageType << STD_LINE_BREAK << text.toStdString();
        emit messageToSend(target, QString::fromStdString(oss.str()));
    }

    /**
     * @brief 注册，返回结果和提示信息。
     *
     * @param target
     * @param name
     * @param password
     */
    void ForumServer::addNormalUser(const QString &target,
        const QString name,
        const QString password)
    {
        User *user;
        QMutexLocker userLocker(&userMutex);
        user = cforum->addNormalUser(name, password);
        if (user)
        {
            sendToast(target, REGISTER_SUCCESS_MESSAGE);
        }
    }

    /**
     * @brief 登录，返回结果和提示信息。
     *
     * @param target
     * @param name
     * @param password
     */
    void ForumServer::login(const QString &target,
        const QString name,
        const QString password)
    {
        if (name == GUEST_NAME && password == GUEST_PASSWORD)
        {
            ostringstream oss;
            oss << UpdateUserResponseMessageType << STD_LINE_BREAK << Guest();
            emit messageToSend(target, QString::fromStdString(oss.str()));
        }
        else
        {
            User *user;
            QMutexLocker userLocker(&userMutex);
            user = cforum->login(name, password);
            if (user)
            {
                ostringstream oss;
                oss << UpdateUserResponseMessageType << STD_LINE_BREAK << *user;
                emit messageToSend(target, QString::fromStdString(oss.str()));
            }
        }
    }

    /**
     * @brief 添加版面，返回结果和提示信息。
     *
     * @param target
     * @param name
     */
    void ForumServer::addBoard(const QString &target, const QString name)
    {
        QMutexLocker contentLocker(&contentMutex);

        Board *board = cforum->addBoard(name);
        if (board)
        {
            sendToast(target, ADD_BOARD_SUCCESS_MESSAGE);
            broadcastBoard(*board);
        }
    }

    /**
     * @brief 发主题帖，返回结果和提示信息。
     *
     * @param target
     * @param boardID
     * @param in
     */
    void ForumServer::addPost(const QString &target, const int boardID, istream &in)
    {
        QMutexLocker userLocker(&userMutex);
        QMutexLocker contentLocker(&contentMutex);

        Post newPost;
        in >> newPost;
        Post *realPost = cforum->addPost(boardID,
            newPost.getTitle(),
            newPost.getContent(),
            newPost.getAuthorID());
        userLocker.unlock();

        if (realPost)
        {
            sendToast(target, ADD_POST_SUCCESS_MESSAGE);
            broadcastPost(boardID, *realPost);
        }
    }

    /**
     * @brief 删主题帖，广播结果，返回提示信息。
     *
     * @param target
     * @param boardID
     * @param postID
     * @param userID
     */
    void ForumServer::removePost(const QString &target,
        const int boardID,
        const int postID,
        const int userID)
    {
        QMutexLocker userLocker(&userMutex);
        QMutexLocker contentLocker(&contentMutex);

        bool result = cforum->removePost(boardID, postID, userID);
        if (result)
        {
            sendToast(target, REMOVE_POST_SUCCESS_MESSAGE);
            broadcastPostDeletion(boardID, postID);
        }
        else
        {
            sendToast(target, REMOVE_POST_ERROR_MESSAGE);
        }
    }

    /**
     * @brief 发回复帖，返回结果和提示信息。
     *
     * @param target
     * @param boardID
     * @param postID
     * @param in
     */
    void ForumServer::addComment(const QString &target,
        const int boardID,
        const int postID,
        istream & in)
    {
        QMutexLocker userLocker(&userMutex);
        QMutexLocker contentLocker(&contentMutex);

        Comment newComment;
        in >> newComment;
        Comment *realComment = cforum->addComment(boardID,
            postID,
            newComment.getContent(),
            newComment.getAuthorID());
        userLocker.unlock();

        if (realComment)
        {
            sendToast(target, ADD_COMMENT_SUCCESS_MESSAGE);
            broadcastComment(boardID, postID, *realComment);
        }
    }

    /**
     * @brief 删回复帖，广播结果，返回提示信息。
     *
     * @param target
     * @param boardID
     * @param postID
     * @param commentID
     * @param userID
     */
    void ForumServer::removeComment(const QString & target,
        const int boardID,
        const int postID,
        const int commentID,
        const int userID)
    {
        QMutexLocker userLocker(&userMutex);
        QMutexLocker contentLocker(&contentMutex);

        bool result = cforum->removeComment(boardID, postID, commentID, userID);
        if (result)
        {
            sendToast(target, REMOVE_COMMENT_SUCCESS_MESSAGE);
            broadcastCommentDeletion(boardID, postID, commentID);
        }
        else
        {
            sendToast(target, REMOVE_COMMENT_ERROR_MESSAGE);
        }
    }

    /**
     * @brief 拷贝地获取客户端地唯一标识符列表。
     *
     * @return ClientHashList*
     */
    ClientHashList * ForumServer::getAllClientHash()
    {
        ClientHashList *hashList = new ClientHashList();
        QMutexLocker clientsLocker(&clientsMutex);
        for (ClientDescriptor *client : *clients)
        {
            hashList->push_back(client->hash());
        }
        return hashList;
    }
}
