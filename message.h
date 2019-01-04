/**
 * @file message.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 消息基类，用户客户端与服务端的交互，其子类实现具体的交互操作。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef CFORUM_MESSAGE_H
#define CFORUM_MESSAGE_H

#include <sstream>

#include <QObject>
#include <QString>

#include "client_state.h"
#include "post.h"

using namespace std;

namespace cforum
{
    enum MessageType
    {
        // 空报文
        EmptyMessageType,
        // 请求报文，请求版面列表，请求前清空版面列表，服务端依次返回所有版面
        GetBoardListRequestMessageType,
        // 请求报文，请求主题帖列表，请求前清空主题帖列表，服务端依次返回所有主题帖
        GetPostListRequestMessageType,
        // 请求报文，请求回复帖列表，请求前清空回复帖列表，服务端依次返回所有回复帖
        GetCommentListRequestMessageType,
        // 请求报文，注册
        RegisterRequestMessageType,
        // 请求报文，登录
        LoginRequestMessageType,
        // 请求报文，注销
        LogoutRequestMessageType,
        // 请求报文，设置版主
        AddModeratorRequestMessageType,
        // 请求报文，撤销版主
        RemoveModeratorRequestMessageType,
        // 响应报文，更新用户信息，仅由服务端发往客户端
        UpdateUserResponseMessageType,
        // 响应报文，显示提示信息
        ToastResponseMessageType,
        // 双向，新增版面
        AddBoardMessageType,
        // 双向，发主题帖
        AddPostMessageType,
        // 双向，发回复帖
        AddCommentMessageType,
        // 双向，删主题帖
        RemovePostMessageType,
        // 双向，删回复帖
        RemoveCommentMessageType,
    };

    class Message : public QObject
    {
        Q_OBJECT

    public:
        Message();
        Message(const QString &qstring);

        virtual void load(const QString &qstring);
        virtual QString dump() const;

        MessageType getType() const;
        QString getMessageString() const;
        void setMessageString(const QString &newMessageString);

    protected:
        MessageType type = EmptyMessageType;
        ClientState clientState;
        QString messageString = "";
    };
}

#endif // CFORUM_MESSAGE_H
