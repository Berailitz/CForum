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
		RequestMessage message;
		message.setModerator(board->getID(), userName);
		sendMessage(message);
	}

	void ClientController::removeModerator(const QString userName)
	{
		RequestMessage message;
		message.removeModerator(board->getID(), userName);
		sendMessage(message);
	}

	void ClientController::viewForum()
	{
		emit openForum();
	}

    void ClientController::addBoard(const QString boardName)
	{
		RequestMessage message;
		message.addBoard(boardName);
		sendMessage(message);
	}

    void ClientController::viewBoard(const int boardID)
    {
		openBoard(boardID);
    }

	bool ClientController::canRemovePost(const int postID) const
	{
		return false;
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
	}

    void ClientController::addComment(const QString content)
	{
		RequestMessage message;
		message.addComment(board->getID(), post->getID(), content, user->getID());
		sendMessage(message);
	}

    void ClientController::removeComment(const int commentID)
	{
	}

	void ClientController::open(const QString &url)
	{
		socket->open(url);
		return;
	}

	void ClientController::onConnected()
	{
		emit messageSent(SERVER_CONNECTED_MESSAGE);
		connect(&*socket, &QWebSocket::textMessageReceived,
			this, &ClientController::onTextMessageReceived);
	}

	void ClientController::onTextMessageReceived(const QString & textMessage)
	{
		ResponseMessage message(textMessage);
		execute(message);
	}

	void ClientController::onDisconnected()
	{
		emit messageSent(SERVER_DISCONNECTED_MESSAGE);
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

	void ClientController::errorRaised(const QString message)
	{
		emit messageSent(message);
	}

	void ClientController::openForum()
	{
		RequestMessage message;
		resetBoards(true);
		resetPosts(true);
		resetComments(true);
		message.getBoardList();
		sendMessage(message);
	}

	void ClientController::openBoard(const int boardID)
	{
		RequestMessage message;
		resetPosts(true);
		resetComments(true);
		message.getPostList(boardID);
		sendMessage(message);
	}

	void ClientController::openPost(const int postID)
	{
		RequestMessage message;
		resetPosts(true);
		resetComments(true);
		message.getPostList(postID);
		sendMessage(message);
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
		switch (message.getType())
		{
		case UpdateUserResponseMessageType:
			loadUser(iss);
			emit forumOpened();
			break;
		case ToastResponseMessageType:
			emit messageSent(messageString);
			break;
		case AddBoardMessageType:
			resetBoards(false);
			newBoard = new Board;
			iss >> *newBoard;
			boards->append(newBoard);
			setBoards();
			emit forumOpened();
			break;
		case AddPostMessageType:
			resetPosts(false);
			newPost = new Post;
			iss >> *newPost;
			posts->append(newPost);
			setPosts();
			emit boardOpened();
			break;
		case AddCommentMessageType:
			resetComments(false);
			newComment = new Comment;
			iss >> *newComment;
			comments->append(newComment);
			setComments();
			emit postOpened();
			break;
		default:
			break;
		}
	}

	void ClientController::sendMessage(RequestMessage & message)
	{
		socket->sendTextMessage(message.dump());
	}

	void ClientController::resetBoards(bool doClear)
	{
		engine.rootContext()->setContextProperty("boardListModel", QVariant::fromValue(*defaultBoards));
		if (doClear)
		{
			for (QObject *&qit : *boards)
			{
				delete static_cast<Board*>(qit);
			}
			boards->clear();
		}
	}

	void ClientController::resetPosts(bool doClear)
	{
		engine.rootContext()->setContextProperty("postListModel", QVariant::fromValue(*defaultPosts));
		if (doClear)
		{
			for (QObject *qit : *posts)
			{
				delete static_cast<Post*>(qit);
			}
			posts->clear();
		}
	}

	void ClientController::resetComments(bool doClear)
	{
		engine.rootContext()->setContextProperty("commentListModel", QVariant::fromValue(*defaultComments));
		if (doClear)
		{
			for (QObject* &qit : *comments)
			{
				Comment *cit;
				cit = static_cast<Comment*>(qit);
				delete cit;
			}
			comments->clear();
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
