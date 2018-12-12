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

	void ClientController::sendMessage(RequestMessage & message)
	{
		socket->sendTextMessage(message.dump());
	}
}
