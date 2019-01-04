#include "client_descriptor.h"

namespace cforum
{
    ClientDescriptor::ClientDescriptor(QWebSocket *socket) :
        ClientState(),
        socket(socket)
    {
        hashString = hashSocket(*socket);
    }

    ClientDescriptor::~ClientDescriptor()
    {
        socket->deleteLater();
    }

    QString ClientDescriptor::hash() const
    {
        return hashString;
    }

    void ClientDescriptor::send(const QString &textMessage)
    {
        socket->sendTextMessage(textMessage);
    }

    QWebSocket * ClientDescriptor::getSocket() const
    {
        return socket;
    }

    bool operator==(const ClientDescriptor & lhs, const ClientDescriptor & rhs)
    {
        return lhs.hash() == rhs.hash();
    }

    bool operator==(const ClientDescriptor & lhs, const QWebSocket & rhs)
    {
        return lhs.hash() == hashSocket(rhs);
    }

    /**
     * @brief 获取连接的唯一字符串表示。
     *
     * @param socket
     * @return QString
     */
    QString hashSocket(const QWebSocket &socket)
    {
        return socket.peerAddress().toString() +
            QString::fromUtf8(":") + QString::number(socket.peerPort());
    }
}
