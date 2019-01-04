/**
 * @file response_message.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 定义应答消息类，用户服务端向客户端发送应答信息。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef CFORUM_RESPONSE_MESSAGE_H
#define CFORUM_RESPONSE_MESSAGE_H

#include "message.h"

namespace cforum
{
    class ResponseMessage : public Message
    {
    public:
        ResponseMessage();
        ResponseMessage(const QString &qstring);
    };
}

#endif // CFORUM_RESPONSE_MESSAGE_H
