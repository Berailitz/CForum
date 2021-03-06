/**
 * @file request_message.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 定义请求消息类，用于客户端向服务端发送请求。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

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

        void addPost(const int boardID,
            const QString title,
            const QString content,
            const int userID);
        void removePost(const int boardID, const int postID, const int userID);

        void addComment(const int boardID,
            const int postID,
            const QString content,
            const int userID);
        void removeComment(const int boardID,
            const int postID,
            const int commentID,
            const int userID);

        void getBoardList();
        void getPostList(const int boardID);
        void getCommentList(const int boardID, const int postID);

        void setModerator(const int boardID,
            const QString userName,
            const int userID);
        void removeModerator(const int boardID,
            const QString userName,
            const int userID);
    };
}

#endif // CFORUM_REQUEST_MESSAGE_H