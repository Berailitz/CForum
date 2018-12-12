#ifndef CFORUM_REQUEST_MESSAGE_H
#define CFORUM_REQUEST_MESSAGE_H

#include "message.h"

namespace cforum
{
	class RequestMessage : public Message
	{
	public:
		RequestMessage();
		void addNormalUser(const QString userName, const QString password);
		void login(const QString userName, const QString password);
		void logout();
		void addBoard(const QString boardName);
		void addPost(const int boardID, const QString title, const QString content, const int userID);
		void addComment(const int boardID, const int postID, const QString content, const int userID);
		void setModerator(const int boardID, const QString userName);
		void removeModerator(const int boardID, const QString userName);
	};
}

#endif // CFORUM_REQUEST_MESSAGE_H