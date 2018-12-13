#include "client_descriptor.h"

namespace cforum
{
    ClientDescriptor::ClientDescriptor(QWebSocket *socket) :
		ClientState(),
		socket(socket)
    {

    }

	ClientDescriptor::~ClientDescriptor()
	{
		socket->deleteLater();
	}

	QString ClientDescriptor::hash() const
	{
		return hashSocket(*socket);
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

	QString hashSocket(const QWebSocket & socket)
	{
		return socket.peerAddress().toString() + QString::fromUtf8(":") + QString::number(socket.peerPort());
	}
}
