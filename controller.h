#ifndef CFORUM_CONTROLLER_H
#define CFORUM_CONTROLLER_H

#include "cforum.h"

namespace cforum
{
    class Controller : public QObject
    {
        Q_OBJECT
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
        bool addBoard(const QString boardName);
		bool postThread(const string title, const string content);
		bool deleteThread(const int threadID);
		bool postComment(const string content);
		bool deleteComment(const int commentID);
		bool load(const string path);
		bool save(const string path) const;
	private:
		User *findUser(const string userName);
	};
}

#endif // !CFORUM_CONTROLLER_H
