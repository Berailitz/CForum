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
		bool registerUser(const QString userName, const QString password);
		bool login(const QString userName, const QString password);
		bool setModerator(const QString userName);
        bool addBoard(const QString boardName);
		bool postThread(const QString title, const QString content);
		bool deleteThread(const int threadID);
		bool postComment(const QString content);
		bool deleteComment(const int commentID);
		bool load(const QString path);
		bool save(const QString path) const;
	private:
		User *findUser(const QString userName);
	};
}

#endif // !CFORUM_CONTROLLER_H
