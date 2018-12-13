#ifndef CFORUM_FORUM_SERVER_H
#define CFORUM_FORUM_SERVER_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QWebSocket>
#include <QWebSocketServer>

#include "cforum.h"
#include "request_message.h"
#include "client_descriptor.h"

namespace cforum
{
	using ClientList = QVector<ClientDescriptor *>;

    class ForumServer : public QObject
    {
        Q_OBJECT
    public:
		ForumServer();
		~ForumServer();
        bool listen(const int port = 8118);

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
		void execute(const RequestMessage &message);
    };
}

#endif // CFORUM_FORUM_SERVER_H
