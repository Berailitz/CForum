#ifndef CFORUM_CONTROLLER_H
#define CFORUM_CONTROLLER_H

#include "cforum.h"

namespace cforum
{
    class Controller : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString greeting READ getGreeting)
	public:
		Controller();
		~Controller();
		CForum *cforum;
		User *user;
		Board *board;
		Thread *thread;
        QString getGreeting() const;
    Q_SIGNALS:
        void logined();
        void boardOpened();
    public Q_SLOTS:
        void registerUser(const QString userName, const QString password);
        void login(const QString userName, const QString password);
        void setModerator(const QString userName);
        void addBoard(const QString boardName);
        void viewBoard(const int boardID);
        void postThread(const QString title, const QString content);
        void deleteThread(const int threadID);
        void postComment(const QString content);
        void deleteComment(const int commentID);
        void load(const QString path);
        void save(const QString path) const;
	private:
		User *findUser(const QString userName);
	};
}

#endif // !CFORUM_CONTROLLER_H
