#ifndef CFORUM_FORUM_SERVER_H
#define CFORUM_FORUM_SERVER_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QWebSocket>
#include <QWebSocketServer>

namespace cforum
{
    class ForumServer : public QObject
    {
        Q_OBJECT
    public:
        ForumServer();
        bool listen(const int port = 8118);

    public Q_SLOTS:
        void onNewConnection();
        void onTextMessageReceived(const QString &message);
        void onDisconnection();
        void sendMessage(const QString &target, const QString &message);

    Q_SIGNALS:
        void messageReceived(const QString &message);

    private:
        QWebSocketServer *server;
        QList<QWebSocket *> clients;
        QString hashSocket(const QWebSocket &socket);
    };
}

#endif // CFORUM_FORUM_SERVER_H
