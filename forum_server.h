#ifndef CFORUM_FORUM_SERVER_H
#define CFORUM_FORUM_SERVER_H

#include <QDebug>
#include <QObject>
#include <QString>
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

    class ForumServer : public QWebSocketServer
    {
        Q_OBJECT
    public:
		ForumServer();
		~ForumServer();
        bool listen(const int port = 8118);
		bool load(const fs::path path);
		bool save(const fs::path path) const;

    public Q_SLOTS:
        void onNewConnection();
        void onTextMessageReceived(const QString &textMessage);
        void onDisconnection();
		void sendMessage(const QString &target, const QString &textMessage);
		void sendMessage(QWebSocket &socket, const QString &textMessage);

    Q_SIGNALS:
        void messageReceived(const QString &message);

    private:
		CForum *cforum;
		ClientList *clients;
		void execute(QWebSocket &socket, const RequestMessage &request);
		void sendBoard(QWebSocket &socket, const Board &board);
		void sendBoardList(QWebSocket &socket);
		void broadcastBoard(const Board &board);
		void sendPost(QWebSocket &socket, const int boardID, const Post &post);
		void sendPostList(QWebSocket &socket, const int boardID);
		void broadcastPost(const int boardID, const Post &post);
		void sendComment(QWebSocket &socket, const int boardID, const int postID, const Comment &comment);
		void sendCommentList(QWebSocket &socket, const int boardID, const int postID);
		void broadcastComment(const int boardID, const int postID, const Comment &comment);
		void sendToast(QWebSocket &socket, const QString &text);
		void addNormalUser(QWebSocket &socket, const QString name, const QString password);
		void login(QWebSocket &socket, const QString name, const QString password);
		void addBoard(QWebSocket &socket, const QString name);
		void addPost(QWebSocket & socket, const int boardID, istream &in);
		void addComment(QWebSocket & socket, const int boardID, const int postID, istream &in);
    };
}

#endif // CFORUM_FORUM_SERVER_H
