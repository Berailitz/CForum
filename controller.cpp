#include "controller.h"

namespace cforum
{
    Controller::Controller() : QObject(), cforum(new CForum)
	{
	}


	Controller::~Controller()
	{
	}

	bool Controller::registerUser(const string newUserName, const string newPassword)
    {
		user = new User(cforum->users->size() + 1, newUserName, newPassword);
        cforum->users->push_back(*user);
		return true;
	}

	bool Controller::login(const string userName, const string password)
	{
		User *user = findUser(userName);
		if (user->isPasswordCorrect(password))
		{
			Controller::user = user;
			return true;
		}
		return false;
	}

	bool Controller::setModerator(const string userName)
	{
		const User *user = findUser(userName);
		if (user == nullptr)
		{
			return board->removeModerator();
		}
		else
		{
			board->setModerator(user->id);
			return true;
		}
	}

    bool Controller::addBoard(const QString boardName)
	{
		board = new Board(cforum->boards->size() + 1, boardName);
		cforum->boards->push_back(board);
		return true;
	}

	bool Controller::postThread(const string title, const string content)
	{
		thread = new Thread(board->threads->size() + 1, content, user->id, title);
		return board->post(thread);
	}

	bool Controller::deleteThread(const int threadID)
	{
		if (threadID <= board->threads->size())
		{
			return board->remove(threadID);
		}
		else
		{
			return false;
		}
	}

	bool Controller::postComment(const string content)
	{
		return thread->post(new Comment(thread->comments->size() + 1, content));
	}

	bool Controller::deleteComment(const int commentID)
	{
		if (commentID <= thread->comments->size())
		{
			return thread->remove(commentID);
		}
		else
		{
			return false;
		}
	}

	bool Controller::load(const string path)
	{
		return cforum->load(path);
	}

	bool Controller::save(const string path) const
	{
		return cforum->save(path);
	}

	User * Controller::findUser(const string userName)
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
