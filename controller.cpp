#include "controller.h"

namespace cforum
{
    Controller::Controller() : QObject(), cforum(new CForum)
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

    void Controller::registerUser(const QString newUserName, const QString newPassword)
    {
		user = new User(cforum->users->size() + 1, newUserName, newPassword);
        cforum->users->push_back(*user);
	}

    void Controller::login(const QString userName, const QString password)
	{
		User *user = findUser(userName);
		if (user->isPasswordCorrect(password))
		{
            qDebug() << "Login success:" << userName;
            Controller::user = user;
            emit logined();
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
        board = cforum->getBoardByID(boardID);
        if (board)
        {
            emit boardOpened();
        }
    }

    void Controller::postThread(const QString title, const QString content)
	{
		thread = new Thread(board->threads->size() + 1, content, user->id, title);
        board->post(thread);
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
        thread->post(new Comment(thread->comments->size() + 1, content));
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
