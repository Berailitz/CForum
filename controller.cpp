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
        return user->greeting();
    }

    QString Controller::getBoardName() const
	{
        return board->name;
	}

	QString Controller::getPostTitle() const
	{
        return post->title + " -- by: " + getUsername(post->authorID) + ", " + post->getTimeString();
	}

    QString Controller::getPostContent() const
    {
        return post->content;
    }

	int Controller::getUserID() const
	{
		return user->id;
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
        User *newUser = cforum->checkPassword(userName, password);
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
		else if (user->isModerator(board->id))
		{
			emit messageSent(USER_IS_MODERATOR_MESSAGE);
		}
		else
		{
			if (cforum->setModerator(board->id, user->id))
			{
				emit messageSent(SET_SUCCESS_MESSAGE);
			}
			else
			{
				emit messageSent(ILLEGAL_OPERATION_MESSAGE);
			}
		}
		openBoard(board->id);
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
			if (cforum->removeModerator(board->id, user->id))
			{
				emit messageSent(DELETE_SUCCESS_MESSAGE);
			}
			else
			{
				emit messageSent(ILLEGAL_OPERATION_MESSAGE);
			}
		}
		openBoard(board->id);
	}

    QString Controller::getUsername(const int userID) const
    {
        const User *user = cforum->getUserByID(userID);
        if (user != nullptr)
        {
            return user->userName;
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
		board = cforum->addBoard(boardName);
	}

    void Controller::viewBoard(const int boardID)
    {
		openBoard(boardID);
    }

    void Controller::postPost(const QString title, const QString content)
	{
        post = new Post(board->posts->size() + 1, content, user->id, title);
        board->post(post);
        qDebug() << POST_SUCCESS_MESSAGE << post->title;
        emit messageSent(POST_SUCCESS_MESSAGE);
        refreshViews();
        emit postOpened();
	}

	void Controller::viewPost(const int postID)
	{
		openPost(postID);
	}

    void Controller::deletePost(const int postID)
	{
		Post *target = board->getPostByID(postID);
		if (target && canDeletePost(target) && (isModerator() || user->id == target->authorID))
		{
			QQmlContext *ctxt = engine.rootContext();
			ctxt->setContextProperty("postListModel", QVariant::fromValue(*defaultBoard->posts));
			ctxt->setContextProperty("commentListModel", QVariant::fromValue(*defaultPost->getComments()));
            board->remove(postID);
			post = defaultPost;
			qDebug() << DELETE_SUCCESS_MESSAGE << postID;
			emit messageSent(DELETE_SUCCESS_MESSAGE);
            refreshViews();
            emit boardOpened();
        }
		else
		{
			qDebug() << DELETE_FAILED_MESSAGE << postID;
			emit messageSent(DELETE_FAILED_MESSAGE);
		}
	}

    void Controller::postComment(const QString content)
	{
        post->post(content, user->id);
        qDebug() << POST_SUCCESS_MESSAGE << content;
        emit messageSent(POST_SUCCESS_MESSAGE);
        refreshViews();
        emit postOpened();
	}

    void Controller::deleteComment(const int commentID)
	{
		Comment *target = post->getCommentByID(commentID);
		if (target && !target->isDeleted && (isModerator() || user->id == target->authorID))
		{
			engine.rootContext()->setContextProperty("commentListModel", QVariant::fromValue(*defaultPost->getComments()));
			post->remove(commentID);
			qDebug() << DELETE_SUCCESS_MESSAGE << commentID;
			emit messageSent(DELETE_SUCCESS_MESSAGE);
			refreshViews();
			emit postOpened();
		}
		else
		{
			qDebug() << DELETE_FAILED_MESSAGE << commentID;
			emit messageSent(DELETE_FAILED_MESSAGE);
		}
	}

    void Controller::load(const QString path)
	{
        cforum->load(path.toStdString());
	}

    void Controller::save(const QString path) const
	{
        cforum->save(path.toStdString());
	}

	bool Controller::isAdmin() const
	{
		return user->isAdmin();
	}

    bool Controller::isModerator() const
    {
        return cforum->isAdmin(user->id) || board->isModerator(user->id);
    }

    void Controller::refreshViews()
	{
		QQmlContext *ctxt = engine.rootContext();
		ctxt->setContextProperty("postListModel", QVariant::fromValue(*defaultBoard->posts));
		ctxt->setContextProperty("commentListModel", QVariant::fromValue(*defaultPost->getComments()));
        ctxt->setContextProperty("forumController", QVariant::fromValue(&*this));
		ctxt->setContextProperty("boardListModel", QVariant::fromValue(*cforum->getBoards()));
        ctxt->setContextProperty("postListModel", QVariant::fromValue(*board->posts));
		ctxt->setContextProperty("commentListModel", QVariant::fromValue(*post->getComments()));
        qDebug() << "Refresh: " << cforum->getBoards()->size() << " (ALL) Boards " << board->posts->size()
                 << " Posts " << post->visibleCommentCounter << " Comments.";
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
