#ifndef CFORUM_CONTROLLER_H
#define CFORUM_CONTROLLER_H

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "cforum.h"

namespace cforum
{
    class Controller : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString greeting READ getGreeting NOTIFY forumOpened)
        Q_PROPERTY(QString boardTitle READ getBoardTitle NOTIFY boardOpened)
        Q_PROPERTY(QString threadTitle READ getThreadTitle NOTIFY threadOpened)
        Q_PROPERTY(QString threadContent READ getThreadContent NOTIFY threadOpened)
        Q_PROPERTY(bool isModerator READ isModerator NOTIFY boardOpened)
	public:
        Controller(QQmlApplicationEngine &engine);
		~Controller();
        CForum *cforum = new CForum();
        User *user = defaultUser;
        Board *board = defaultBoard;
        Thread *thread = defaultThread;
        QString getGreeting() const;
        QString getBoardTitle() const;
		QString getThreadTitle() const;
        QString getThreadContent() const;
        bool isModerator() const;
        void refreshViews();
    Q_SIGNALS:
        void forumOpened();
        void boardOpened();
        void threadOpened();
        void messageSent(QString message);
    public Q_SLOTS:
        void registerUser(const QString userName, const QString password);
        void login(const QString userName, const QString password);
        void setModerator(const QString userName);
        QString getUsername(const int userID) const;
        void addBoard(const QString boardName);
        void viewBoard(const int boardID);
        void postThread(const QString title, const QString content);
		void viewThread(const int threadID);
        void deleteThread(const int threadID);
        void postComment(const QString content);
        void deleteComment(const int commentID);
        void load(const QString path);
        void save(const QString path) const;
	private:
        QQmlApplicationEngine &engine;
		void openForum();
        void openBoard(const int boardID);
        void openThread(const int threadID);
		User *findUser(const QString userName);
        User *defaultUser = new Guest();
        Board *defaultBoard = new Board();
        Thread *defaultThread = new Thread();
	};
}

#endif // !CFORUM_CONTROLLER_H
