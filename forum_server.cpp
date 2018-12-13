#include "forum_server.h"

namespace cforum
{
    ForumServer::ForumServer() : QObject(), server(new QWebSocketServer(QStringLiteral("WS Server"),
                                                                  QWebSocketServer::NonSecureMode,
                                                                  this))
    {
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
        clients << socket;
    }

    void ForumServer::onTextMessageReceived(const QString &message)
    {
        QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
        emit messageReceived(hashSocket(*socket) + QString::fromUtf8(" > ") + message + "\n");
    }

    void ForumServer::onDisconnection()
    {
        QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
        emit messageReceived(hashSocket(*socket) + " disconnected.\n");
        if (socket)
        {
            clients.removeAll(socket);
            socket->deleteLater();
        }
    }

    void ForumServer::sendMessage(const QString &target, const QString &message)
    {
        for (QWebSocket *client : clients)
        {
            if (hashSocket(*client) == target)
            {
                client->sendTextMessage(message);
                emit messageReceived(target + " < " + message + "\n");
            }
        }
    }

    QString ForumServer::hashSocket(const QWebSocket &socket)
    {
        return socket.peerAddress().toString() + QString::fromUtf8(":") + QString::number(socket.peerPort());
    }
}
