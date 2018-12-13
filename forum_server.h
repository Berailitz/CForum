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

	const string REGISTER_SUCCESS_MESSAGE = "注册成功";

    class ForumServer : public QObject
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

    Q_SIGNALS:
        void messageReceived(const QString &message);

    private:
		CForum *cforum;
        QWebSocketServer *server;
		ClientList *clients;
		void execute(QWebSocket &socket, const RequestMessage &request);
		void sendBoardList(QWebSocket &socket) const;
		void sendPostList(QWebSocket &socket, const int boardID) const;
		void sendCommentList(QWebSocket &socket, const int boardID, const int postID) const;
		void addNormalUser(QWebSocket &socket, const QString name, const QString password) const;
		void login(QWebSocket &socket, const QString name, const QString password) const;
		void addBoard(QWebSocket &socket, const QString name) const;
    };
}

#endif // CFORUM_FORUM_SERVER_H
