#include "controller.h"

namespace cforum
{
    Controller::Controller(QQmlApplicationEngine &engine) : 
		QObject(), engine(engine),
		cforum(new CForum()),
		defaultUser(new Guest()),
		defaultBoard(new Board(-1, "DefaultBoard")),
		defaultPost(new Post(-1, "DefaultPost", -1, "DefaultContent"))
	{
		user = defaultUser;
		board = defaultBoard;
		post = defaultPost;
	}

    Controller::~Controller()
	{
	}

    QString Controller::getGreeting() const
    {
        return WELCOME_MESSAGE + user->getInfo();
    }

    QString Controller::getBoardName() const
	{
        return board->getName();
	}

	QString Controller::getPostTitle() const
	{
        return post->getTitle() + " -- by: " + getUsername(post->getAuthorID()) + ", " + post->getTimeString();
	}

    QString Controller::getPostContent() const
    {
        return post->getContent();
    }

	int Controller::getUserID() const
	{
		return user->getID();
	}

    void Controller::addUser(const QString newUserName, const QString newPassword)
    {
		user = cforum->addNormalUser(newUserName, newPassword);
		qDebug() << REGISTER_SUCCESS_MESSAGE << newUserName << ":" << newPassword;
		emit messageSent(REGISTER_SUCCESS_MESSAGE);
        login(newUserName, newPassword);
	}

    void Controller::login(const QString userName, const QString password)
	{
        User *newUser = cforum->login(userName, password);
        if (newUser)
		{
            qDebug() << LOGIN_SUCCESS_MESSAGE << userName;
            emit messageSent(LOGIN_SUCCESS_MESSAGE);
            Controller::user = newUser;
			openForum();
		}
        else
        {
            qDebug() << LOGIN_FAILED_MESSAGE << userName;
            emit messageSent(LOGIN_FAILED_MESSAGE);
        }
	}

	void Controller::logout()
	{
		cforum->logout(user->getName());
		user = defaultUser;
		board = defaultBoard;
		post = defaultPost;
		refreshViews();
		emit messageSent(LOGOUT_SUCCESS_MESSAGE);
	}

    void Controller::setModerator(const QString userName)
	{
 		const User *user = cforum->getUserByName(userName);
		if (user == nullptr)
		{
			emit messageSent(NO_USER_MESSAGE);
		}
		else if (user->isAdmin())
		{
			emit messageSent(USER_IS_ADMIN_MESSAGE);
		}
		else if (user->isModerator(board->getID()))
		{
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

	void Controller::removeModerator(const QString userName)
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
				emit messageSent(ILLEGAL_OPERATION_MESSAGE);
			}
		}
		openBoard(board->getID());
	}

    QString Controller::getUsername(const int userID) const
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

	void Controller::viewForum()
	{
		emit openForum();
	}

    void Controller::addBoard(const QString boardName)
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

    void Controller::viewBoard(const int boardID)
    {
		openBoard(boardID);
    }

    void Controller::addPost(const QString title, const QString content)
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
			refreshViews();
			emit messageSent(ILLEGAL_OPERATION_MESSAGE);
		}
	}

	void Controller::viewPost(const int postID)
	{
		openPost(postID);
	}

    void Controller::removePost(const int postID)
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
			qDebug() << DELETE_FAILED_MESSAGE << postID;
			refreshViews();
			emit messageSent(DELETE_FAILED_MESSAGE);
		}
	}

    void Controller::addComment(const QString content)
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
			refreshViews();
			emit messageSent(ILLEGAL_OPERATION_MESSAGE);
		}
	}

    void Controller::removeComment(const int commentID)
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
			qDebug() << DELETE_FAILED_MESSAGE << commentID;
			refreshViews();
			emit messageSent(DELETE_FAILED_MESSAGE);
		}
	}

    bool Controller::load(const QString path)
	{
        return cforum->load(path.toStdString());
	}

	bool Controller::save(const QString path) const
	{
        return cforum->save(path.toStdString());
	}

	bool Controller::isAdmin() const
	{
		return user->isAdmin();
	}

    bool Controller::isModerator() const
    {
        return cforum->isAdmin(user->getID()) || board->isModerator(user->getID());
    }

    void Controller::refreshViews()
	{
		QQmlContext *ctxt = engine.rootContext();
		ctxt->setContextProperty("postListModel", QVariant::fromValue(*defaultBoard->getPosts()));
		ctxt->setContextProperty("commentListModel", QVariant::fromValue(*defaultPost->getComments()));
        ctxt->setContextProperty("forumController", QVariant::fromValue(&*this));
		ctxt->setContextProperty("boardListModel", QVariant::fromValue(*cforum->getBoards()));
        ctxt->setContextProperty("postListModel", QVariant::fromValue(*board->getPosts()));
		ctxt->setContextProperty("commentListModel", QVariant::fromValue(*post->getComments()));
        qDebug() << "Refresh: " << cforum->getBoards()->size() << " (ALL) Boards ";
	}

	bool Controller::canDeletePost(Post * target) const
	{
		return target->canDelete();
	}

	void Controller::openForum()
	{
		emit forumOpened();
	}

	void Controller::openBoard(const int boardID)
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

	void Controller::openPost(const int postID)
	{
		Post *newPost = board->getPostByID(postID);
		if (newPost)
		{
			post = newPost;
			refreshViews();
			emit postOpened();
		}
	}
}
