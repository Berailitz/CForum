#ifndef CFORUM_FORUM_SERVER_H
#define CFORUM_FORUM_SERVER_H

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QString>
#include <QtConcurrent>
#include <QWebSocket>
#include <QWebSocketServer>

#include "cforum.h"
#include "request_message.h"
#include "response_message.h"
#include "client_descriptor.h"

namespace cforum
{
	using ClientList = QVector<ClientDescriptor *>;

	const QString REGISTER_SUCCESS_MESSAGE = QString::fromUtf8("注册成功");
	const QString ADD_BOARD_SUCCESS_MESSAGE = QString::fromUtf8("添加版面成功");
	const QString ADD_POST_SUCCESS_MESSAGE = QString::fromUtf8("发主题帖成功");
	const QString ADD_COMMENT_SUCCESS_MESSAGE = QString::fromUtf8("发回复帖成功");
	const fs::path DEFAULT_DATABASE_FOLDER_PATH = "data";

    class ForumServer : public QWebSocketServer
    {
        Q_OBJECT
    public:
		ForumServer();
		~ForumServer();
        bool listen(const int port = 8118);
		bool load(const fs::path path);
		bool save() const;
		bool save(const fs::path path) const;

    public Q_SLOTS:
        void onNewConnection();
        void onTextMessageReceived(const QString &textMessage);
        void onDisconnection();
		void onMessageToSend(QWebSocket &socket, const QString &textMessage);
		void onMessageToSend(const QString &target, const QString &textMessage);

    Q_SIGNALS:
		void messageReceived(const QString &message);
		void messageToSend(QWebSocket &socket, const QString &textMessage);
		void messageToSend(const QString &target, const QString &textMessage);

    private:
		QMutex mutex;
		CForum *cforum;
		ClientList *clients;
		fs::path lastPath = DEFAULT_DATABASE_FOLDER_PATH;
		void execute(const QString &target, const RequestMessage &request);
		void sendBoard(const QString &target, const Board &board);
		void sendBoardList(const QString &target);
		void broadcastBoard(const Board &board);
		void sendPost(const QString &target, const int boardID, const Post &post);
		void sendPostList(const QString &target, const int boardID);
		void broadcastPost(const int boardID, const Post &post);
		void sendComment(const QString &target, const int boardID, const int postID, const Comment &comment);
		void sendCommentList(const QString &target, const int boardID, const int postID);
		void broadcastComment(const int boardID, const int postID, const Comment &comment);
		void sendToast(const QString &target, const QString &text);
		void addNormalUser(const QString &target, const QString name, const QString password);
		void login(const QString &target, const QString name, const QString password);
		void addBoard(const QString &target, const QString name);
		void addPost(const QString &target, const int boardID, istream &in);
		void addComment(const QString &target, const int boardID, const int postID, istream &in);
    };
}

#endif // CFORUM_FORUM_SERVER_H
