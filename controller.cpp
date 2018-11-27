#include "controller.h"

namespace cforum
{
    Controller::Controller(QQmlApplicationEngine &engine) : QObject(), engine(engine)
	{
	}

    Controller::~Controller()
	{
	}

    QString Controller::getGreeting() const
    {
        return "欢迎 " + user->userName + " !";
    }

    QString Controller::getBoardTitle() const
	{
        return board->name + "(" + QString::fromStdString(to_string(board->threads->size())) + ")";
	}

	QString Controller::getThreadTitle() const
	{
        return thread->title + " -- by: " + getUsername(thread->authorID);
	}

    QString Controller::getThreadContent() const
    {
        return thread->content;
    }

    void Controller::registerUser(const QString newUserName, const QString newPassword)
    {
		user = new User(cforum->users->size() + 1, newUserName, newPassword);
        cforum->users->push_back(*user);
        login(newUserName, newPassword);
	}

    void Controller::login(const QString userName, const QString password)
	{
        User *newUser = findUser(userName);
        if (newUser && newUser->isPasswordCorrect(password))
		{
            qDebug() << "Login success:" << userName;
            emit messageSent("Welcome.");
            Controller::user = newUser;
			openForum();
		}
        else
        {
            qDebug() << "Login failed:" << userName;
            emit messageSent("Login failed.");
        }
	}

    void Controller::setModerator(const QString userName)
	{
		const User *user = findUser(userName);
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
        qDebug() << "New Thread: " << thread->title;
        emit messageSent("Thread posted.");
        refreshViews();
        emit threadOpened();
	}

	void Controller::viewThread(const int threadID)
	{
		openThread(threadID);
	}

    void Controller::deleteThread(const int threadID)
	{
		if (threadID > 0 && threadID <= board->threads->size())
		{
            board->remove(threadID);
			thread = defaultThread;
            refreshViews();
            emit boardOpened();
        }
	}

    void Controller::postComment(const QString content)
	{
        Comment *newComment = new Comment(thread->comments->size() + 1, content, user->id);
        thread->post(newComment);
        qDebug() << "New Comment: " << newComment->content;
        emit messageSent("Comment posted.");
        refreshViews();
        emit threadOpened();
	}

    void Controller::deleteComment(const int commentID)
	{
		if (commentID > 0 && commentID <= thread->comments->size())
		{
            thread->remove(commentID);
            refreshViews();
            emit threadOpened();
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

    bool Controller::isModerator() const
    {
        return cforum->isAdmin(user->id) || board->isModerator(user->id);
    }

    void Controller::refreshViews()
	{
		QQmlContext *ctxt = engine.rootContext();
        ctxt->setContextProperty("forumController", QVariant::fromValue(&*this));
		ctxt->setContextProperty("boardListModel", QVariant::fromValue(*cforum->boards));
        ctxt->setContextProperty("threadListModel", QVariant::fromValue(*board->threads));
		ctxt->setContextProperty("commentListModel", QVariant::fromValue(*thread->comments));
        qDebug() << "Refresh: " << cforum->boards->size() << " Boards " << board->threads->size()
                 << " Threads " << thread->comments->size() << " Comments.";
	}

	void Controller::openForum()
	{
		emit forumOpened();
	}

	void Controller::openBoard(const int boardID)
	{
		Board *newBoard = cforum->getBoardByID(boardID);
		if (newBoard != nullptr)
		{
			board = newBoard;
			refreshViews();
			emit boardOpened();
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

	User * Controller::findUser(const QString userName)
	{
		for (User &user : *cforum->users)
		{
			if (user.userName == userName)
			{
				return &user;
			}
		}
		return nullptr;
	}
}
