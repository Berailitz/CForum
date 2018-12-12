#ifndef CFORUM_CLIENT_STATE_H
#define CFORUM_CLIENT_STATE_H

#include <QObject>

namespace cforum
{
	class ClientState : public QObject
	{
		Q_OBJECT
	public:
		explicit ClientState();

	public:
		int userID = -1;
		int boardID = -1;
		int postID = -1;
		int commentID = -1;
	};
}

#endif // CFORUM_CLIENT_STATE_H
