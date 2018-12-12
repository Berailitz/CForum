#ifndef CFORUM_CONTROLLER_H
#define CFORUM_CONTROLLER_H

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QWebSocket>
#include "cforum.h"
#include "response_message.h"
#include "request_message.h"

namespace cforum
{
	const QString WELCOME_MESSAGE = QString::fromUtf8("欢迎 ");
	const QString REGISTER_SUCCESS_MESSAGE = QString::fromUtf8("注册成功");
	const QString REGISTER_FAILED_MESSAGE = QString::fromUtf8("注册失败");
	const QString LOGIN_SUCCESS_MESSAGE = QString::fromUtf8("登录成功");
	const QString LOGIN_FAILED_MESSAGE = QString::fromUtf8("登录失败：用户名或密码错误");
	const QString LOGOUT_SUCCESS_MESSAGE = QString::fromUtf8("注销成功");
	const QString POST_SUCCESS_MESSAGE = QString::fromUtf8("发布成功");
	const QString SET_SUCCESS_MESSAGE = QString::fromUtf8("设置成功");
	const QString DELETE_SUCCESS_MESSAGE = QString::fromUtf8("删除成功");
	const QString DELETE_FAILED_MESSAGE = QString::fromUtf8("删除失败");
	const QString NO_USER_MESSAGE = QString::fromUtf8("用户不存在");
	const QString USER_IS_ADMIN_MESSAGE = QString::fromUtf8("用户是管理员");
	const QString USER_IS_MODERATOR_MESSAGE = QString::fromUtf8("用户是版主");
	const QString ILLEGAL_OPERATION_MESSAGE = QString::fromUtf8("非法操作");
	const QString MODERATOR_NOTE_MESSAGE = QString::fromUtf8("（我管理的版面）");
	const QString MODERATOR_LIST_PREFIX_MESSAGE = QString::fromUtf8("，版主为：");
	const QString NO_MODERATOR_MESSGAE = QString::fromUtf8("，本版面没有版主");
	const QString LIST_SEPARATER = QString::fromUtf8(", ");
	const QUrl DATABASE_PATH = QUrl::fromLocalFile("data");
	const QString LOADING_DATABASE_MESSAGE = QString::fromUtf8("加载中");
	const QString SAVING_DATABASE_MESSAGE = QString::fromUtf8("保存中");
	const QString SERVER_CONNECTED_MESSAGE = QString::fromUtf8("连接服务器成功");
	const QString SERVER_DISCONNECTED_MESSAGE = QString::fromUtf8("连接已断开");

    class ClientController : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString greeting READ getGreeting NOTIFY forumOpened)
        Q_PROPERTY(QString boardTitle READ getBoardTitle NOTIFY boardOpened)
        Q_PROPERTY(QString postTitle READ getPostTitle NOTIFY postOpened)
        Q_PROPERTY(QString postContent READ getPostContent NOTIFY postOpened)
        Q_PROPERTY(bool isAdmin READ isAdmin NOTIFY forumOpened)
		Q_PROPERTY(bool isGuest READ isGuest NOTIFY boardOpened)
		Q_PROPERTY(bool isModerator READ isModerator NOTIFY boardOpened)
		Q_PROPERTY(int userID READ getUserID NOTIFY forumOpened)
	public:
        ClientController(QQmlApplicationEngine &engine);
		~ClientController();
        QString getGreeting() const;
        QString getBoardTitle() const;
		QString getPostTitle() const;
        QString getPostContent() const;
		int getUserID() const;
		bool isAdmin() const;
		bool isGuest() const;
		bool isModerator() const;
        void refreshViews(); // 刷新UI
    Q_SIGNALS:
        void forumOpened();
        void boardOpened();
        void postOpened();
        void messageSent(QString message);
    public Q_SLOTS:
		void errorRaised(const QString message);
		void initializeConnection();
		void initializeDatabase();
        void addUser(const QString userName, const QString password); // 注册
        void login(const QString userName, const QString password);
		void guestLogin();
		void logout();
        void setModerator(const QString userName); // 将用户设为当前版面的版主之一
		void removeModerator(const QString userName); // 将用户从当前版面的版主列表中移除
        QString getUsername(const int userID) const; // #TODO#
		void viewForum(); // 控制UI转向论坛界面
        void addBoard(const QString boardName);
        void viewBoard(const int boardID = -1); // 控制UI转向版面界面
		bool canRemovePost(const int postID) const;
        void addPost(const QString title, const QString content); // 发主题帖
		void viewPost(const int postID); // 控制UI转向主题帖界面
        void removePost(const int postID); // 删除主题帖
        void addComment(const QString content); // 发回复帖
        void removeComment(const int commentID); // 删除回复帖
		void open(const QString &url = QString::fromUtf8("ws://localhost:8118/"));
		void onConnected();
		void onTextMessageReceived(const QString &textMessage);
		void onDisconnected();
	private:
        QQmlApplicationEngine &engine;
		QWebSocket *socket = nullptr;
		CForum *cforum;
		User *user;
		Board *board;
		Post *post;
		void openForum();
        void openBoard(const int boardID);
        void openPost(const int postID);
        User *defaultUser;
        Board *defaultBoard;
        Post *defaultPost;
		void execute(ResponseMessage &message);
		void sendMessage(RequestMessage &message);
	};
}

#endif // !CFORUM_CONTROLLER_H
