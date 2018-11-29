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
	const QString NO_USER_MESSAGE = QString::fromUtf8("用户不存在"); 
	const QString NO_BOARD_MESSAGE = QString::fromUtf8("版面不存在");
	const QString NO_THREAD_MESSAGE = QString::fromUtf8("主题帖不存在");

    class Controller : public QObject
    {
        Q_OBJECT
	public:
        Controller(QQmlApplicationEngine *engine = nullptr);
		~Controller();
        CForum *cforum;
		void addBoard(const QString boardName);
    Q_SIGNALS:
        void messageSent(QString message);
		void forumOpened();
    public Q_SLOTS:
		// 此处的函数默认接受可传递的不安全的参数
		QString getUsername(const int userID) const;
        User *registerUser(const QString userName, const QString password);
		QObject *login(const QString userName, const QString password);
		void setModerator(const int boardID, const QString userName);
		Thread *getThread(const int boardID, const int threadID) const;
        void postThread(const int userID, const int boardID, const QString title, const QString content);
        void deleteThread(const int userID, const int boardID, const int threadID);
        void postComment(const int userID, const int boardID, const int threadID, const QString content);
        void deleteComment(const int userID, const int boardID, const int threadID, const int commentID);
        void load(const QString path);
        void save(const QString path) const;
	private:
        QQmlApplicationEngine *engine;
		bool canDeleteThread(const User *user, const Board *board, const Thread *thread);
		bool canDeleteComment(const User *user, const Board *board, const Thread *thread, const Comment *comment);
		User *getUserByName(const QString userName);
        User *defaultUser;
        Board *defaultBoard;
        Thread *defaultThread;
	};
}

#endif // !CFORUM_CONTROLLER_H
