#ifndef CFORUM_CLIENT_DESCRIPTOR_H
#define CFORUM_CLIENT_DESCRIPTOR_H

#include <QObject>
#include <QWebSocket>

#include "client_state.h"

namespace cforum
{
    class ClientDescriptor : public ClientState
    {
        Q_OBJECT
    public:
		ClientDescriptor(QWebSocket *socket);
		~ClientDescriptor();
		QString hash() const;
		void send(const QString &textMessage);
		QWebSocket *getSocket() const;
		friend bool operator==(const ClientDescriptor& lhs, const ClientDescriptor& rhs);

	private:
		QWebSocket *socket;
    };

	bool operator==(const ClientDescriptor& lhs, const ClientDescriptor& rhs);

	QString hashSocket(const QWebSocket &socket);
}

#endif // CFORUM_CLIENT_DESCRIPTOR_H
