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
