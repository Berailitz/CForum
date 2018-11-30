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
	const QString SET_SUCCESS_MESSAGE = QString::fromUtf8("设置成功");
	const QString DELETE_SUCCESS_MESSAGE = QString::fromUtf8("删除成功");
	const QString DELETE_FAILED_MESSAGE = QString::fromUtf8("删除失败");
	const QString NO_USER_MESSAGE = QString::fromUtf8("用户不存在");
	const QString USER_IS_ADMIN_MESSAGE = QString::fromUtf8("用户是管理员");
	const QString USER_IS_MODERATOR_MESSAGE = QString::fromUtf8("用户是版主");
	const QString ILLEGAL_OPERATION_MESSAGE = QString::fromUtf8("非法操作");

    class Controller : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString greeting READ getGreeting NOTIFY forumOpened)
        Q_PROPERTY(QString boardName READ getBoardName NOTIFY boardOpened)
        Q_PROPERTY(QString postTitle READ getPostTitle NOTIFY postOpened)
        Q_PROPERTY(QString postContent READ getPostContent NOTIFY postOpened)
        Q_PROPERTY(bool isAdmin READ isAdmin NOTIFY boardOpened)
		Q_PROPERTY(bool isModerator READ isModerator NOTIFY boardOpened)
		Q_PROPERTY(int userID READ getUserID NOTIFY forumOpened)
	public:
        Controller(QQmlApplicationEngine &engine);
		~Controller();
        CForum *cforum;
        User *user;
        Board *board;
        Post *post;
        QString getGreeting() const;
        QString getBoardName() const;
		QString getPostTitle() const;
        QString getPostContent() const;
		int getUserID() const;
        bool isAdmin() const;
		bool isModerator() const;
        void refreshViews();
    Q_SIGNALS:
        void forumOpened();
        void boardOpened();
        void postOpened();
        void messageSent(QString message);
    public Q_SLOTS:
        void addUser(const QString userName, const QString password);
        void login(const QString userName, const QString password);
        void setModerator(const QString userName);
		void removeModerator(const QString userName);
        QString getUsername(const int userID) const;
		void viewForum();
        void addBoard(const QString boardName);
        void viewBoard(const int boardID = -1);
        void postPost(const QString title, const QString content);
		void viewPost(const int postID);
        void deletePost(const int postID);
        void postComment(const QString content);
        void deleteComment(const int commentID);
        void load(const QString path);
        void save(const QString path) const;
	private:
        QQmlApplicationEngine &engine;
		bool canDeletePost(Post *target) const;
		void openForum();
        void openBoard(const int boardID);
        void openPost(const int postID);
        User *defaultUser;
        Board *defaultBoard;
        Post *defaultPost;
	};
}

#endif // !CFORUM_CONTROLLER_H
