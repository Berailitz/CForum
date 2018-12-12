#include "client_controller.h"

namespace cforum
{
    ClientController::ClientController(QQmlApplicationEngine &engine) : 
		QObject(), engine(engine),
		cforum(new CForum()),
		defaultUser(new Guest()),
		defaultBoard(new Board(-1, "DefaultBoard")),
		defaultPost(new Post(-1, "DefaultPost", -1, "DefaultContent"))
	{
		// 设置缺省值，以供UI层使用
		user = defaultUser;
		board = defaultBoard;
		post = defaultPost;
		initializeConnection();
	}

    ClientController::~ClientController()
	{
		delete cforum;
		delete defaultUser;
		delete defaultBoard;
		delete defaultPost;
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
			boardTitle += MODERATOR_LIST_PREFIX_MESSAGE;
			int moderatorIndex = 1;
			for (ModeratorSet::iterator mit = moderators->begin(); mit != moderators->end(); mit++)
			{
				if (moderatorIndex == moderators->size())
				{
					boardTitle += cforum->getUserByID(*mit)->getName();
				}
				else
				{
					boardTitle += (cforum->getUserByID(*mit)->getName() + LIST_SEPARATER);
				}
				moderatorIndex++;
			}
		}
        return boardTitle;
	}

	QString ClientController::getPostTitle() const
	{
        return post->getTitle() + " -- by: " + getUsername(post->getAuthorID()) + ", " + post->getTimeString();
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

	void ClientController::initializeDatabase()
	{
		cforum->initializeDatabase();
	}

    void ClientController::addUser(const QString newUserName, const QString newPassword)
    {
		user = cforum->addNormalUser(newUserName, newPassword);
		if (user)
		{
			qDebug() << REGISTER_SUCCESS_MESSAGE << newUserName << ":" << newPassword;
			emit messageSent(REGISTER_SUCCESS_MESSAGE);
			//login(newUserName, newPassword); 不自动登录
		}
		else
		{
			qDebug() << REGISTER_FAILED_MESSAGE << newUserName << ":" << newPassword;
			emit messageSent(REGISTER_FAILED_MESSAGE);
		}
	}

    void ClientController::login(const QString userName, const QString password)
	{
        User *newUser = cforum->login(userName, password);
        if (newUser)
		{
            qDebug() << LOGIN_SUCCESS_MESSAGE << userName;
            emit messageSent(LOGIN_SUCCESS_MESSAGE);
            ClientController::user = newUser;
			openForum();
		}
        else
        {
            qDebug() << LOGIN_FAILED_MESSAGE << userName;
            emit messageSent(LOGIN_FAILED_MESSAGE);
        }
	}

	void ClientController::guestLogin()
	{
		qDebug() << LOGIN_SUCCESS_MESSAGE;
		emit messageSent(LOGIN_SUCCESS_MESSAGE);
		openForum();
	}

	void ClientController::logout()
	{
		cforum->logout(user->getName());
		user = defaultUser;
		board = defaultBoard;
		post = defaultPost;
		refreshViews();
		emit messageSent(LOGOUT_SUCCESS_MESSAGE);
	}

    void ClientController::setModerator(const QString userName)
	{
 		const User *user = cforum->getUserByName(userName);
		if (user == nullptr)
		{
			emit messageSent(NO_USER_MESSAGE);
		}
		else if (user->isAdmin())
		{
			// 用户是管理员，不能被设置为版主
			emit messageSent(USER_IS_ADMIN_MESSAGE);
		}
		else if (user->isModerator(board->getID()))
		{
			// 用户已经是该版面的版主了
			emit messageSent(USER_IS_MODERATOR_MESSAGE);
		}
		else
		{
			if (cforum->setModerator(board->getID(), user->getID()))
			{
				emit messageSent(SET_SUCCESS_MESSAGE);
			}
			else
			{
				emit messageSent(ILLEGAL_OPERATION_MESSAGE);
			}
		}
		openBoard(board->getID());
	}

	void ClientController::removeModerator(const QString userName)
	{
		const User *user = cforum->getUserByName(userName);
		if (user == nullptr)
		{
			emit messageSent(NO_USER_MESSAGE);
		}
		else
		{
			if (cforum->removeModerator(board->getID(), user->getID()))
			{
				emit messageSent(DELETE_SUCCESS_MESSAGE);
			}
			else
			{
				// 用户不是该版面的版主
				emit messageSent(ILLEGAL_OPERATION_MESSAGE);
			}
		}
		openBoard(board->getID());
	}

    QString ClientController::getUsername(const int userID) const
    {
        const User *user = cforum->getUserByID(userID);
        if (user != nullptr)
        {
            return user->getName();
        }
        else
        {
            return "";
        }
    }

	void ClientController::viewForum()
	{
		emit openForum();
	}

    void ClientController::addBoard(const QString boardName)
	{
		Board *newBoard = cforum->addBoard(boardName);
		if (newBoard)
		{
			board = newBoard;
			refreshViews();
		}
		else
		{
			// TODO throw exception
		}
	}

    void ClientController::viewBoard(const int boardID)
    {
		openBoard(boardID);
    }

	bool ClientController::canRemovePost(const int postID) const
	{
		return cforum->canRemovePost(board->getID(), postID, user->getID());
	}

    void ClientController::addPost(const QString title, const QString content)
	{
		QQmlContext *ctxt = engine.rootContext();
		ctxt->setContextProperty("postListModel", QVariant::fromValue(*defaultBoard->getPosts()));
        post = cforum->addPost(board->getID(), title, content, user->getID());
		if (post)
		{
			qDebug() << POST_SUCCESS_MESSAGE << post->getTitle();
			refreshViews();
			emit messageSent(POST_SUCCESS_MESSAGE);
			emit postOpened();
		}
		else
		{
			// 非法操作，即用户不能发主题帖
			refreshViews();
			emit messageSent(ILLEGAL_OPERATION_MESSAGE);
		}
	}

	void ClientController::viewPost(const int postID)
	{
		openPost(postID);
	}

    void ClientController::removePost(const int postID)
	{
		QQmlContext *ctxt = engine.rootContext();
		ctxt->setContextProperty("postListModel", QVariant::fromValue(*defaultBoard->getPosts()));
		ctxt->setContextProperty("commentListModel", QVariant::fromValue(*defaultPost->getComments()));
		if (cforum->removePost(board->getID(), postID, user->getID()))
		{
			post = defaultPost;
			qDebug() << DELETE_SUCCESS_MESSAGE << postID;
			refreshViews();
			emit messageSent(DELETE_SUCCESS_MESSAGE);
			emit boardOpened();
		}
		else
		{
			// 非法操作，即用户不能删该主题贴
			qDebug() << DELETE_FAILED_MESSAGE << postID;
			refreshViews();
			emit messageSent(DELETE_FAILED_MESSAGE);
		}
	}

    void ClientController::addComment(const QString content)
	{
		engine.rootContext()->setContextProperty("commentListModel", QVariant::fromValue(*defaultPost->getComments()));
		if (cforum->addComment(board->getID(), post->getID(), content, user->getID()))
		{
			qDebug() << POST_SUCCESS_MESSAGE << content;
			emit messageSent(POST_SUCCESS_MESSAGE);
			refreshViews();
			emit postOpened();
		}
		else
		{
			// 非法操作，即用户发回复贴
			refreshViews();
			emit messageSent(ILLEGAL_OPERATION_MESSAGE);
		}
	}

    void ClientController::removeComment(const int commentID)
	{
		engine.rootContext()->setContextProperty("commentListModel", QVariant::fromValue(*defaultPost->getComments()));
		if (cforum->removeComment(board->getID(), post->getID(), commentID, user->getID()))
		{
			qDebug() << DELETE_SUCCESS_MESSAGE << commentID;
			emit messageSent(DELETE_SUCCESS_MESSAGE);
			refreshViews();
			emit postOpened();
		}
		else
		{
			// 非法操作，即用户不能删该回复帖
			qDebug() << DELETE_FAILED_MESSAGE << commentID;
			refreshViews();
			emit messageSent(DELETE_FAILED_MESSAGE);
		}
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
		ctxt->setContextProperty("boardListModel", QVariant::fromValue(*cforum->getBoards()));
        ctxt->setContextProperty("postListModel", QVariant::fromValue(*board->getPosts()));
		ctxt->setContextProperty("commentListModel", QVariant::fromValue(*post->getComments()));
        qDebug() << "Refresh: " << cforum->getBoards()->size() << " (ALL) Boards ";
	}

	void ClientController::errorRaised(const QString message)
	{
		emit messageSent(message);
	}

	void ClientController::openForum()
	{
		emit forumOpened();
	}

	void ClientController::openBoard(const int boardID)
	{
		if (boardID == -1)
		{
			refreshViews();
			emit boardOpened();
		}
		else
		{
			Board *newBoard = cforum->getBoardByID(boardID);
			if (newBoard != nullptr)
			{
				board = newBoard;
				refreshViews();
				emit boardOpened();
			}
		}
	}

	void ClientController::openPost(const int postID)
	{
		Post *newPost = board->getPostByID(postID);
		if (newPost)
		{
			post = newPost;
			refreshViews();
			emit postOpened();
		}
	}

	void ClientController::execute(ResponseMessage & message)
	{
		emit messageSent(message.dump());
	}
}
