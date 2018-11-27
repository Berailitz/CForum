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
        QString greeting = "Welcome ";
        if (user == nullptr)
        {
            greeting += "Guest";
        }
        else
        {
            greeting += user->userName;
        }
        greeting += "!";
        return greeting;
    }

    QString Controller::getBoardHeader() const
	{
        return board->name + "(" + QString::fromStdString(to_string(board->threads->size())) + ")";
	}

	QString Controller::getThreadTitle() const
	{
		return thread->title;
	}

    QString Controller::getThreadContent() const
    {
        return thread->content;
    }

    void Controller::registerUser(const QString newUserName, const QString newPassword)
    {
		user = new User(cforum->users->size() + 1, newUserName, newPassword);
        cforum->users->push_back(*user);
        emit forumOpened();
	}

    void Controller::login(const QString userName, const QString password)
	{
		User *user = findUser(userName);
		if (user->isPasswordCorrect(password))
		{
            qDebug() << "Login success:" << userName;
            Controller::user = user;
            emit forumOpened();
		}
        else
        {
            qDebug() << "Login failed:" << userName;
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
	}

    void Controller::addBoard(const QString boardName)
	{
		board = new Board(cforum->boards->size() + 1, boardName);
        cforum->boards->push_back(board);
	}

    void Controller::viewBoard(const int boardID)
    {
        Board *newBoard = cforum->getBoardByID(boardID);
        if (newBoard != nullptr)
        {
            board = newBoard;
            refreshViews();
            emit boardOpened();
        }
    }

    void Controller::postThread(const QString title, const QString content)
	{
        thread = new Thread(board->threads->size() + 1, content, user->id, title);
        board->post(thread);
        qDebug() << "New Thread: " << thread->title;
        refreshViews();
        emit threadOpened();
	}

	void Controller::viewThread(const int threadID)
	{
        Thread *newThread = board->getThreadByID(threadID);
        if (newThread)
		{
            thread = newThread;
            refreshViews();
            emit threadOpened();
		}
	}

    void Controller::deleteThread(const int threadID)
	{
		if (threadID <= board->threads->size())
		{
            board->remove(threadID);
        }
	}

    void Controller::postComment(const QString content)
	{
        Comment *newComment = new Comment(thread->comments->size() + 1, content);
        thread->post(newComment);
        qDebug() << "New Comment: " << newComment->content;
        refreshViews();
        emit threadOpened();
	}

    void Controller::deleteComment(const int commentID)
	{
		if (commentID <= thread->comments->size())
		{
            thread->remove(commentID);
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
