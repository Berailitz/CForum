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
        // 设置缺省值，以供UI层使用
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

    QString ClientController::getBoardTitle() const
    {
        QString boardTitle = board->getName() + (isModerator() ? MODERATOR_NOTE_MESSAGE : "");
        ModeratorSet* moderators = board->getModerators();
        if (moderators->size() == 0)
        {
            boardTitle += NO_MODERATOR_MESSGAE;
        }
        else
        {
            boardTitle += (MODERATOR_COUNT_PREFIX_MESSAGE + QString::number(moderators->size()) + MODERATOR_COUNT_SUBFIX_MESSAGE);
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

    void ClientController::initializeConnection()
    {
        if (socket)
        {
            delete socket;
        }
        socket = new QWebSocket();
        QObject::connect(&*socket, &QWebSocket::connected, this, &ClientController::onConnected);
        QObject::connect(&*socket, &QWebSocket::disconnected, this, &ClientController::onDisconnected);
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

    void ClientController::setModerator(const QString userName)
    {
    }

    void ClientController::removeModerator(const QString userName)
    {
    }

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
        emit messageSent(SERVER_CONNECTING_MESSAGE);
        if (socket->state() == QAbstractSocket::UnconnectedState)
        {
            socket->open(url);
        }
        return;
    }

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
            emit messageSent(SERVER_DISCONNECTED_MESSAGE + socket->errorString());
            connect();
        }
    }

    void ClientController::onError()
    {
        emit messageSent(SERVER_ERROR_MESSAGE + socket->errorString());
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

    void ClientController::refreshViews()
    {
        // 刷新UI
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

    void ClientController::openForum()
    {
        RequestMessage message;
        message.getBoardList();
        sendMessage(message);
        board = defaultBoard;
    }

    void ClientController::openBoard(const int boardID)
    {
        RequestMessage message;
        message.getPostList(boardID);
        sendMessage(message);
        resetBoards(false);
        board = static_cast<Board*>(boards->at(boardID - 1));
        post = defaultPost;
    }

    void ClientController::openPost(const int postID)
    {
        RequestMessage message;
        message.getCommentList(board->getID(), postID);
        sendMessage(message);
        resetPosts(false);
        post = static_cast<Post*>(posts->at(postID - 1));
        setPosts();
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

    void ClientController::sendMessage(RequestMessage & message)
    {
        socket->sendTextMessage(message.dump());
    }

    void ClientController::clearBoards()
    {
        board = defaultBoard;
        for (QObject *&qit : *boards)
        {
            qit->deleteLater();
        }
        boards->clear();
    }

    void ClientController::clearPosts()
    {
        post = defaultPost;
        for (QObject *qit : *posts)
        {
            qit->deleteLater();
        }
        posts->clear();
    }

    void ClientController::clearComments()
    {
        for (QObject* &qit : *comments)
        {
            qit->deleteLater();
        }
        comments->clear();
    }

    void ClientController::resetBoards(bool doClear)
    {
        engine.rootContext()->setContextProperty("boardListModel", QVariant::fromValue(*defaultBoards));
        if (doClear)
        {
            clearBoards();
        }
    }

    void ClientController::resetPosts(bool doClear)
    {
        engine.rootContext()->setContextProperty("postListModel", QVariant::fromValue(*defaultPosts));
        if (doClear)
        {
            clearPosts();
        }
    }

    void ClientController::resetComments(bool doClear)
    {
        engine.rootContext()->setContextProperty("commentListModel", QVariant::fromValue(*defaultComments));
        if (doClear)
        {
            clearComments();
        }
    }

    void ClientController::setBoards()
    {
        engine.rootContext()->setContextProperty("boardListModel", QVariant::fromValue(*boards));
    }

    void ClientController::setPosts()
    {
        engine.rootContext()->setContextProperty("postListModel", QVariant::fromValue(*posts));
    }

    void ClientController::setComments()
    {
        engine.rootContext()->setContextProperty("commentListModel", QVariant::fromValue(*comments));
    }
}
