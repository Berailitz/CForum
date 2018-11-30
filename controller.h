#ifndef CFORUM_CONTROLLER_H
#define CFORUM_CONTROLLER_H

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "cforum.h"

namespace cforum
{
	const QString REGISTER_SUCCESS_MESSAGE = QString::fromUtf8("注册成功");
	const QString LOGIN_SUCCESS_MESSAGE = QString::fromUtf8("登录成功");
	const QString LOGIN_FAILED_MESSAGE = QString::fromUtf8("登录失败：用户名或密码错误");
	const QString POST_SUCCESS_MESSAGE = QString::fromUtf8("发布成功");
	const QString DELETE_SUCCESS_MESSAGE = QString::fromUtf8("删除成功");
	const QString DELETE_FAILED_MESSAGE = QString::fromUtf8("删除失败");

    class Controller : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString greeting READ getGreeting NOTIFY forumOpened)
        Q_PROPERTY(QString boardTitle READ getBoardTitle NOTIFY boardOpened)
        Q_PROPERTY(QString threadTitle READ getThreadTitle NOTIFY threadOpened)
        Q_PROPERTY(QString threadContent READ getThreadContent NOTIFY threadOpened)
        Q_PROPERTY(bool isAdmin READ isAdmin NOTIFY boardOpened)
		Q_PROPERTY(bool isModerator READ isModerator NOTIFY boardOpened)
		Q_PROPERTY(int userID READ getUserID NOTIFY forumOpened)
	public:
        Controller(QQmlApplicationEngine &engine);
		~Controller();
        CForum *cforum;
        User *user;
        Board *board;
        Thread *thread;
        QString getGreeting() const;
        QString getBoardTitle() const;
		QString getThreadTitle() const;
        QString getThreadContent() const;
		int getUserID() const;
        bool isAdmin() const;
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
		void viewForum();
        void addBoard(const QString boardName);
        void viewBoard(const int boardID = -1);
        void postThread(const QString title, const QString content);
		void viewThread(const int threadID);
        void deleteThread(const int threadID);
        void postComment(const QString content);
        void deleteComment(const int commentID);
        void load(const QString path);
        void save(const QString path) const;
	private:
        QQmlApplicationEngine &engine;
		bool canDeleteThread(Thread *target) const;
		void openForum();
        void openBoard(const int boardID);
        void openThread(const int threadID);
		User *getUserByName(const QString userName);
        User *defaultUser;
        Board *defaultBoard;
        Thread *defaultThread;
	};
}

#endif // !CFORUM_CONTROLLER_H
