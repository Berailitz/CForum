#ifndef CFORUM_CONTROLLER_H
#define CFORUM_CONTROLLER_H

#include "cforum.h"

namespace cforum
{
	class Controller
	{
	public:
		Controller();
		~Controller();
		CForum *cforum;
		User *user;
		Board *board;
		Thread *thread;
		bool registerUser(const string userName, const string password);
		bool login(const string userName, const string password);
		bool setModerator(const string userName);
		bool postThread(const string title, const string content);
		bool deleteThread(const int threadID);
		bool postComment(const string content);
		bool deleteComment(const int commentID);
	private:
		User *findUser(const string userName);
	};
}

#endif // !CFORUM_CONTROLLER_H
