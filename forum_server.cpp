#include "forum_server.h"

namespace cforum
{
	ForumServer::ForumServer() :
		QObject(),
		cforum(new CForum()),
		server(new QWebSocketServer(QStringLiteral("WS Server"),
			QWebSocketServer::NonSecureMode,
			this)),
		clients(new QVector<ClientDescriptor *>)
    {
    }

	ForumServer::~ForumServer()
	{
		delete cforum;
		delete server;
		for (ClientDescriptor * client : *clients)
		{
			client->getSocket()->close();
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
		ClientDescriptor *newClient = new ClientDescriptor(socket);
        emit messageReceived(newClient->hash() + " connected.\n");

        connect(socket, &QWebSocket::textMessageReceived,
                this, &ForumServer::onTextMessageReceived);
        connect(socket, &QWebSocket::disconnected,
                this, &ForumServer::onDisconnection);
        *clients << newClient;
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
		for (auto *cit = clients->begin(); cit != clients->end(); cit++)
		{
			if ((*cit)->hash() == hashSocket(*socket))
			{
				clients->erase(cit);
				break;
			}
		}
        emit messageReceived(hashSocket(*socket) + " disconnected.\n");
    }

    void ForumServer::sendMessage(const QString &target, const QString &textMessage)
    {
        for (ClientDescriptor *client : *clients)
        {
            if (client->hash() == target)
            {
                client->send(textMessage);
                emit messageReceived(target + " < " + textMessage + "\n");
            }
        }
    }

	void ForumServer::execute(const RequestMessage & message)
	{
	}
}
