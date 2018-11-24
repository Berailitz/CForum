#include "controller.h"

namespace cforum
{
	Controller::Controller() : cforum(new CForum)
	{
	}


	Controller::~Controller()
	{
	}

	bool Controller::registerUser(const string newUserName, const string newPassword)
    {
        cforum->users->push_back(User(cforum->users->size() + 1, newUserName, newPassword));
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
		return thread->post(new Comment(board->threads->size() + 1, content));
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
