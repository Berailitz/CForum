#include "controller.h"

namespace cforum
{
    Controller::Controller(QQmlApplicationEngine &engine) : 
		QObject(), engine(engine),
		cforum(new CForum()),
		defaultUser(new Guest()),
		defaultBoard(new Board(-1, "DefaultBoard")),
		defaultThread(new Thread(-1, "DefaultThread", -1, "DefaultContent"))
	{
		user = defaultUser;
		board = defaultBoard;
		thread = defaultThread;
	}

    Controller::~Controller()
	{
	}

    QString Controller::getGreeting() const
    {
        return user->greeting();
    }

    QString Controller::getBoardTitle() const
	{
        return board->name + "(" + QString::fromStdString(to_string(board->threads->size())) + QString::fromUtf8("篇帖子)");
	}

	QString Controller::getThreadTitle() const
	{
        return thread->title + " -- by: " + getUsername(thread->authorID) + ", " + thread->getTimeString();
	}

    QString Controller::getThreadContent() const
    {
        return thread->content;
    }

	int Controller::getUserID() const
	{
		return user->id;
	}

    void Controller::registerUser(const QString newUserName, const QString newPassword)
    {
		user = new NormalUser(cforum->users->size() + 1, newUserName, newPassword);
        cforum->users->push_back(user);
		qDebug() << REGISTER_SUCCESS_MESSAGE << newUserName << ":" << newPassword;
		emit messageSent(REGISTER_SUCCESS_MESSAGE);
        login(newUserName, newPassword);
	}

    void Controller::login(const QString userName, const QString password)
	{
        User *newUser = getUserByName(userName);
        if (newUser && newUser->isPasswordCorrect(password))
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
		const User *user = getUserByName(userName);
		if (user == nullptr)
		{
            board->removeModerator();
		}
		else
		{
            board->setModerator(user->id);
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
		board = new Board(cforum->boards->size() + 1, boardName);
        cforum->boards->push_back(board);
	}

    void Controller::viewBoard(const int boardID)
    {
		openBoard(boardID);
    }

    void Controller::postThread(const QString title, const QString content)
	{
        thread = new Thread(board->threads->size() + 1, content, user->id, title);
        board->post(thread);
        qDebug() << POST_SUCCESS_MESSAGE << thread->title;
        emit messageSent(POST_SUCCESS_MESSAGE);
        refreshViews();
        emit threadOpened();
	}

	void Controller::viewThread(const int threadID)
	{
		openThread(threadID);
	}

    void Controller::deleteThread(const int threadID)
	{
		Thread *target = board->getThreadByID(threadID);
		if (target && canDeleteThread(target) && (isModerator() || user->id == target->authorID))
		{
			QQmlContext *ctxt = engine.rootContext();
			ctxt->setContextProperty("threadListModel", QVariant::fromValue(*defaultBoard->threads));
			ctxt->setContextProperty("commentListModel", QVariant::fromValue(*defaultThread->comments));
            board->remove(threadID);
			thread = defaultThread;
			qDebug() << DELETE_SUCCESS_MESSAGE << threadID;
			emit messageSent(DELETE_SUCCESS_MESSAGE);
            refreshViews();
            emit boardOpened();
        }
		else
		{
			qDebug() << DELETE_FAILED_MESSAGE << threadID;
			emit messageSent(DELETE_FAILED_MESSAGE);
		}
	}

    void Controller::postComment(const QString content)
	{
        Comment *newComment = new Comment(thread->comments->size() + 1, content, user->id);
        thread->post(newComment);
        qDebug() << POST_SUCCESS_MESSAGE << newComment->content;
        emit messageSent(POST_SUCCESS_MESSAGE);
        refreshViews();
        emit threadOpened();
	}

    void Controller::deleteComment(const int commentID)
	{
		Comment *target = thread->getCommentByID(commentID);
		if (target && !target->isDeleted && (isModerator() || user->id == target->authorID))
		{
			engine.rootContext()->setContextProperty("commentListModel", QVariant::fromValue(*defaultThread->comments));
			thread->remove(commentID);
			qDebug() << DELETE_SUCCESS_MESSAGE << commentID;
			emit messageSent(DELETE_SUCCESS_MESSAGE);
			refreshViews();
			emit threadOpened();
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
		return cforum->isAdmin(user->id);
	}

    bool Controller::isModerator() const
    {
        return cforum->isAdmin(user->id) || board->isModerator(user->id);
    }

    void Controller::refreshViews()
	{
		QQmlContext *ctxt = engine.rootContext();
		ctxt->setContextProperty("threadListModel", QVariant::fromValue(*defaultBoard->threads));
		ctxt->setContextProperty("commentListModel", QVariant::fromValue(*defaultThread->comments));
        ctxt->setContextProperty("forumController", QVariant::fromValue(&*this));
		ctxt->setContextProperty("boardListModel", QVariant::fromValue(*cforum->boards));
        ctxt->setContextProperty("threadListModel", QVariant::fromValue(*board->threads));
		ctxt->setContextProperty("commentListModel", QVariant::fromValue(*thread->comments));
        qDebug() << "Refresh: " << cforum->boards->size() << " Boards " << board->threads->size()
                 << " Threads " << thread->comments->size() << " Comments.";
	}

	bool Controller::canDeleteThread(Thread * target) const
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

	void Controller::openThread(const int threadID)
	{
		Thread *newThread = board->getThreadByID(threadID);
		if (newThread)
		{
			thread = newThread;
			refreshViews();
			emit threadOpened();
		}
	}

	User * Controller::getUserByName(const QString userName)
	{
		for (User *user : *cforum->users)
		{
			if (user->userName == userName)
			{
				return user;
			}
		}
		return nullptr;
	}
}
