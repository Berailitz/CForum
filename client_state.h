#ifndef CFORUM_CLIENT_STATE_H
#define CFORUM_CLIENT_STATE_H

#include <QObject>

namespace cforum
{
	class ClientState : public QObject
	{
		Q_OBJECT
	public:
		ClientState();
		ClientState(const ClientState &oldClientState);
		ClientState(const ClientState *oldClientState);
		virtual ~ClientState();
		void initialize(const ClientState *oldClientState);
		ClientState operator=(const ClientState &oldClientState);

	public:
		int userID = -1;
		int boardID = -1;
		int postID = -1;
		int commentID = -1;
	};
}

#endif // CFORUM_CLIENT_STATE_H
