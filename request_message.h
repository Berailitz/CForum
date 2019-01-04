#ifndef CFORUM_REQUEST_MESSAGE_H
#define CFORUM_REQUEST_MESSAGE_H

#include "message.h"

namespace cforum
{
    class RequestMessage : public Message
    {
    public:
        RequestMessage();
        RequestMessage(const RequestMessage& oldRequestMessage);
        RequestMessage(const RequestMessage* oldRequestMessage);
        RequestMessage(const QString &qstring);
        void initialize(const RequestMessage* oldRequestMessage);
        void addNormalUser(const QString userName, const QString password);
        void login(const QString userName, const QString password);
        void logout();
        void addBoard(const QString boardName);
        void addPost(const int boardID, const QString title, const QString content, const int userID);
        void removePost(const int boardID, const int postID, const int userID);
        void addComment(const int boardID, const int postID, const QString content, const int userID);
        void removeComment(const int boardID, const int postID, const int commentID, const int userID);
        void getBoardList();
        void getPostList(const int boardID);
        void getCommentList(const int boardID, const int postID);
    };
}

#endif // CFORUM_REQUEST_MESSAGE_H