#include "client_controller.h"

namespace cforum
{
    ClientController::ClientController(QQmlApplicationEngine &engine) :
        QObject(), engine(engine),
        defaultUser(new Guest()),
        defaultBoard(new Board(-1, "DefaultBoard")),
        defaultPost(new Post(-1, "DefaultPost", -1, "DefaultContent")),
        defaultBoards(new BoardList()),
        defaultPosts(new PostList()),
        defaultComments(new CommentList())
    {
        // 设置缺省值，以供view层使用
        user = defaultUser;
        board = defaultBoard;
        post = defaultPost;
        boards = defaultBoards;
        posts = defaultPosts;
        comments = defaultComments;
        initializeConnection();
        QObject::connect(&*socket, &QWebSocket::textMessageReceived,
            this, &ClientController::onTextMessageReceived);
    }

    ClientController::~ClientController()
    {
        if (user != defaultUser)
        {
            delete user;
        }
        if (board != defaultBoard)
        {
            delete board;
        }
        if (post != defaultPost)
        {
            delete post;
        }
        if (boards != defaultBoards)
        {
            delete boards;
        }
        if (posts != defaultPosts)
        {
            delete posts;
        }
        if (comments != defaultComments)
        {
            delete comments;
        }
        delete defaultUser;
        delete defaultBoard;
        delete defaultPost;
        delete defaultBoards;
        delete defaultPosts;
        delete defaultComments;
    }

    QString ClientController::getGreeting() const
    {
        return WELCOME_MESSAGE + user->getInfo() + QString::fromUtf8("。");
    }

    /**
     * @brief 获取当前版面的（广义）标题，标题包含作者信息。
     *
     * @return QString
     */
    QString ClientController::getBoardTitle() const
    {
        QString boardTitle = board->getName();
        ModeratorSet* moderators = board->getModerators();

        if (isModerator())
        {
            boardTitle += MODERATOR_NOTE_MESSAGE;
        }

        if (moderators->size() == 0)
        {
            boardTitle += NO_MODERATOR_MESSGAE;
        }
        else
        {
            boardTitle += MODERATOR_COUNT_PREFIX_MESSAGE;
            boardTitle += QString::number(moderators->size());
            boardTitle += MODERATOR_COUNT_SUBFIX_MESSAGE;
        }
        return boardTitle;
    }

    QString ClientController::getPostTitle() const
    {
        return post->getTitle();
    }

    QString ClientController::getPostContent() const
    {
        return post->getContent();
    }

    int ClientController::getUserID() const
    {
        return user->getID();
    }

    /**
     * @brief 建立与服务器的连接，并连接信号和槽。
     *
     */
    void ClientController::initializeConnection()
    {
        if (socket)
        {
            delete socket;
        }
        socket = new QWebSocket();
        QObject::connect(&*socket,
            &QWebSocket::connected,
            this,
            &ClientController::onConnected);
        QObject::connect(&*socket,
            &QWebSocket::disconnected,
            this,
            &ClientController::onDisconnected);
    }

    void ClientController::addUser(const QString newUserName, const QString newPassword)
    {
        RequestMessage message;
        message.addNormalUser(newUserName, newPassword);
        sendMessage(message);
    }

    void ClientController::login(const QString userName, const QString password)
    {
        RequestMessage message;
        message.login(userName, password);
        sendMessage(message);
    }

    void ClientController::guestLogin()
    {
        RequestMessage message;
        message.login(GUEST_NAME, GUEST_PASSWORD);
        sendMessage(message);
    }

    void ClientController::logout()
    {
        RequestMessage message;
        message.logout();
        sendMessage(message);
    }

    /**
     * @brief 设置版主。
     *
     * @param userName
     */
    void ClientController::setModerator(const QString userName)
    {
        RequestMessage message;
        message.setModerator(board->getID(), userName, user->getID());
        sendMessage(message);
    }

    /**
     * @brief 撤销版主。
     *
     * @param userName
     */
    void ClientController::removeModerator(const QString userName)
    {
        RequestMessage message;
        message.removeModerator(board->getID(), userName, user->getID());
        sendMessage(message);
    }

    /**
     * @brief 供View层调用，转到论坛首页。
     *
     */
    void ClientController::viewForum()
    {
        openForum();
    }

    void ClientController::addBoard(const QString boardName)
    {
        RequestMessage message;
        message.addBoard(boardName);
        sendMessage(message);
    }

    void ClientController::viewBoard(const int boardID)
    {
        if (boardID == -1)
        {
            openBoard(board->getID());
        }
        else
        {
            openBoard(boardID);
        }
    }

    bool ClientController::canRemovePost(const int postID) const
    {
        return true;
    }

    void ClientController::addPost(const QString title, const QString content)
    {
        RequestMessage message;
        message.addPost(board->getID(), title, content, user->getID());
        sendMessage(message);
    }

    /**
     * @brief 供View层调用，转到帖子页。
     *
     * @param postID
     */
    void ClientController::viewPost(const int postID)
    {
        openPost(postID);
    }

    void ClientController::removePost(const int postID)
    {
        RequestMessage message;
        message.removePost(board->getID(), postID, user->getID());
        sendMessage(message);
    }

    void ClientController::addComment(const QString content)
    {
        RequestMessage message;
        message.addComment(board->getID(), post->getID(), content, user->getID());
        sendMessage(message);
    }

    void ClientController::removeComment(const int commentID)
    {
        RequestMessage message;
        message.removeComment(board->getID(), post->getID(), commentID, user->getID());
        sendMessage(message);
    }

    void ClientController::connect(const QString &url)
    {
        autoReconnect = true;
        emit appOpened();
        emit messageSent(SERVER_CONNECTING_MESSAGE);
        if (socket->state() == QAbstractSocket::UnconnectedState)
        {
            socket->open(url);
        }
        return;
    }

    /**
     * @brief 停止自动重连，并断开与服务端的连接。
     *
     */
    void ClientController::disconnect()
    {
        autoReconnect = false;
        socket->close();
        emit messageSent(SERVER_DISCONNECT_MESSAGE);
    }

    void ClientController::onConnected()
    {
        emit messageSent(SERVER_CONNECTED_MESSAGE);
    }

    void ClientController::onTextMessageReceived(const QString & textMessage)
    {
        ResponseMessage message(textMessage);
        execute(message);
    }

    void ClientController::onDisconnected()
    {
        if (autoReconnect)
        {
            emit messageSent(SERVER_DISCONNECTED_MESSAGE);
            connect();
        }
    }

    void ClientController::onError()
    {
        emit messageSent(SERVER_ERROR_MESSAGE);
        connect();
    }

    bool ClientController::isAdmin() const
    {
        return user->isAdmin();
    }

    bool ClientController::isGuest() const
    {
        return user->isGuest();
    }

    bool ClientController::isModerator() const
    {
        return board->isModerator(user->getID());
    }

    /**
     * @brief 刷新UI。
     *
     */
    void ClientController::refreshViews()
    {
        QQmlContext *ctxt = engine.rootContext();
        ctxt->setContextProperty("forumController", QVariant::fromValue(&*this));
        ctxt->setContextProperty("boardListModel", QVariant::fromValue(*boards));
        ctxt->setContextProperty("postListModel", QVariant::fromValue(*posts));
        ctxt->setContextProperty("commentListModel", QVariant::fromValue(*comments));
        qDebug() << "Refresh: " << boards << " (ALL) Boards ";
    }

    void ClientController::onErrorRaised(const QString message)
    {
        emit messageSent(message);
    }

    /**
     * @brief 转至论坛首页，供控制器controller层调用。
     *
     */
    void ClientController::openForum()
    {
        RequestMessage message;
        message.getBoardList();
        sendMessage(message);
        board = defaultBoard;
    }

    /**
     * @brief 转至版面页，供控制器controller层调用。
     *
     * @param boardID
     */
    void ClientController::openBoard(const int boardID)
    {
        RequestMessage message;
        message.getPostList(boardID);
        sendMessage(message);

        resetBoards(false);
        resetPosts(false);

        board = static_cast<Board*>(boards->at(boardID - 1));
        post = defaultPost;
        clearPosts();

        setPosts();
        setBoards();
        emit boardOpened();
    }

    /**
     * @brief 转至主题帖页，供控制器controller层调用。
     *
     * @param postID
     */
    void ClientController::openPost(const int postID)
    {
        RequestMessage message;
        message.getCommentList(board->getID(), postID);
        sendMessage(message);


        resetBoards(false);
        resetPosts(false);
        resetComments(false);

        post = static_cast<Post*>(posts->at(postID - 1));
        clearComments();

        setComments();
        setPosts();
        setBoards();
        emit postOpened();
    }

    void ClientController::loadUser(istream &userStream)
    {
        if (user != defaultUser)
        {
            delete user;
        }
        user = cforum::loadUser(userStream);
    }

    /**
     * @brief 执行服务器发来的应答消息。
     *
     * @param message
     */
    void ClientController::execute(ResponseMessage & message)
    {
        QString messageString = message.getMessageString();
        istringstream iss(messageString.toStdString());
        Board *newBoard;
        Post *newPost;
        Comment *newComment;
        int targetBoardID, targetPostID, targetCommentID;
        switch (message.getType())
        {
        case UpdateUserResponseMessageType:
            loadUser(iss);
            openForum();
            break;
        case ToastResponseMessageType:
            emit messageSent(messageString);
            break;
        case AddBoardMessageType:
            resetBoards(false);
            newBoard = new Board;
            iss >> *newBoard;
            if (newBoard->getID() == 1)
            {
                clearBoards();
            }
            boards->append(newBoard);
            setBoards();
            emit forumOpened();
            break;
        case AddPostMessageType:
            iss >> targetBoardID;
            iss.get();
            resetBoards(false);
            if (board->getID() == targetBoardID)
            {
                resetPosts(false);

                newPost = new Post;
                iss >> *newPost;
                if (newPost->getID() == 1)
                {
                    clearPosts();
                }
                posts->append(newPost);

                setPosts();
                emit boardOpened();
            }
            setBoards();
            break;
        case AddCommentMessageType:
            iss >> targetBoardID;
            iss >> targetPostID;
            iss.get();
            resetBoards(false);
            resetPosts(false);
            if (board->getID() == targetBoardID && post->getID() == targetPostID)
            {
                resetComments(false);

                newComment = new Comment;
                iss >> *newComment;
                if (newComment->getID() == 1)
                {
                    clearComments();
                }
                comments->append(newComment);

                setComments();
                emit postOpened();
            }
            setPosts();
            setBoards();
            break;
        case RemovePostMessageType:
            iss >> targetBoardID;
            iss >> targetPostID;
            iss.get();
            resetBoards(false);
            if (board->getID() == targetBoardID)
            {
                resetPosts(false);

                PostList::iterator pit = posts->begin();
                advance(pit, targetPostID - 1);
                posts->erase(pit);

                setPosts();
                emit boardOpened();
            }
            setBoards();
            break;
        case RemoveCommentMessageType:
            iss >> targetBoardID;
            iss >> targetPostID;
            iss >> targetCommentID;
            iss.get();
            resetBoards(false);
            resetPosts(false);
            if (board->getID() == targetBoardID && post->getID() == targetPostID)
            {
                resetComments(false);

                CommentList::iterator cit = comments->begin();
                advance(cit, targetCommentID - 1);
                comments->erase(cit);

                setComments();
                emit postOpened();
            }
            setPosts();
            setBoards();
            break;
        default:
            break;
        }
    }

    /**
     * @brief 向服务器发送消息的唯一方法。
     *
     * @param message
     */
    void ClientController::sendMessage(RequestMessage & message)
    {
        socket->sendTextMessage(message.dump());
    }

    /**
     * @brief 初始化版面列表。
     *
     */
    void ClientController::clearBoards()
    {
        board = defaultBoard;
        for (QObject *&qit : *boards)
        {
            qit->deleteLater();
        }
        boards->clear();
    }

    /**
     * @brief 初始化主题帖列表。
     *
     */
    void ClientController::clearPosts()
    {
        post = defaultPost;
        for (QObject *qit : *posts)
        {
            qit->deleteLater();
        }
        posts->clear();
    }

    /**
     * @brief 初始化回复帖列表。
     *
     */
    void ClientController::clearComments()
    {
        for (QObject* &qit : *comments)
        {
            qit->deleteLater();
        }
        comments->clear();
    }

    /**
     * @brief 断开版面列表与view层的连接，使其可以被控制器层访问。
     *
     * @param doClear
     */
    void ClientController::resetBoards(bool doClear)
    {
        engine.rootContext()->setContextProperty("boardListModel",
            QVariant::fromValue(*defaultBoards));
        if (doClear)
        {
            clearBoards();
        }
    }

    /**
     * @brief 断开主题帖列表与view层的连接，使其可以被控制器层访问。
     *
     * @param doClear
     */
    void ClientController::resetPosts(bool doClear)
    {
        engine.rootContext()->setContextProperty("postListModel",
            QVariant::fromValue(*defaultPosts));
        if (doClear)
        {
            clearPosts();
        }
    }

    /**
     * @brief 断开回复帖列表与view层的连接，使其可以被控制器层访问。
     *
     * @param doClear
     */
    void ClientController::resetComments(bool doClear)
    {
        engine.rootContext()->setContextProperty("commentListModel",
            QVariant::fromValue(*defaultComments));
        if (doClear)
        {
            clearComments();
        }
    }

    /**
     * @brief 接通版面帖列表与view层的连接，使view层更新列表的信息。
     *
     */
    void ClientController::setBoards()
    {
        engine.rootContext()->setContextProperty("boardListModel",
            QVariant::fromValue(*boards));
    }

    /**
     * @brief 接通主题帖列表与view层的连接，使view层更新列表的信息。
     *
     */
    void ClientController::setPosts()
    {
        engine.rootContext()->setContextProperty("postListModel",
            QVariant::fromValue(*posts));
    }

    /**
     * @brief 接通回复帖列表与view层的连接，使view层更新列表的信息。
     *
     */
    void ClientController::setComments()
    {
        engine.rootContext()->setContextProperty("commentListModel",
            QVariant::fromValue(*comments));
    }
}
