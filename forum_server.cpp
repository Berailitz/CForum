#include "forum_server.h"

namespace cforum
{
	ForumServer::ForumServer() :
		QObject(),
		cforum(new CForum()),
		server(new QWebSocketServer(QStringLiteral("WS Server"),
			QWebSocketServer::NonSecureMode,
			this)),
		clients(new QList<QWebSocket *>)
    {
    }

	ForumServer::~ForumServer()
	{
		delete cforum;
		delete server;
		for (QWebSocket * client : *clients)
		{
			client->close();
			client->deleteLater();
		}
		delete clients;
	}

	bool ForumServer::listen(const int port)
	{
		if (server->listen(QHostAddress::Any, port))
		{
			emit messageReceived("Listening on " + QString::number(port) + "\n");
			connect(server, &QWebSocketServer::newConnection,
				this, &ForumServer::onNewConnection);
			return true;
		}
		else
		{
			emit messageReceived("CANNOT listening on " + QString::number(port) + "\n");
			return false;
		}
	}

    void ForumServer::onNewConnection()
    {
        QWebSocket *socket = server->nextPendingConnection();
        emit messageReceived(hashSocket(*socket) + " connected.\n");
        socket->setParent(this);

        connect(socket, &QWebSocket::textMessageReceived,
                this, &ForumServer::onTextMessageReceived);
        connect(socket, &QWebSocket::disconnected,
                this, &ForumServer::onDisconnection);
        *clients << socket;
    }

    void ForumServer::onTextMessageReceived(const QString &textMessage)
    {
        QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
		RequestMessage message(textMessage);
		execute(message);
        emit messageReceived(hashSocket(*socket) + QString::fromUtf8(" > ") + textMessage + "\n");
    }

    void ForumServer::onDisconnection()
    {
        QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
        emit messageReceived(hashSocket(*socket) + " disconnected.\n");
        if (socket)
        {
			clients->removeAll(socket);
            socket->deleteLater();
        }
    }

    void ForumServer::sendMessage(const QString &target, const QString &textMessage)
    {
        for (QWebSocket *client : *clients)
        {
            if (hashSocket(*client) == target)
            {
                client->sendTextMessage(textMessage);
                emit messageReceived(target + " < " + textMessage + "\n");
            }
        }
    }

    QString ForumServer::hashSocket(const QWebSocket &socket)
    {
        return socket.peerAddress().toString() + QString::fromUtf8(":") + QString::number(socket.peerPort());
    }

	void ForumServer::execute(const RequestMessage & message)
	{
	}
}
