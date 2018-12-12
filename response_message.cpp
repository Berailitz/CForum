#include "response_message.h"

namespace cforum
{
	ResponseMessage::ResponseMessage() : Message()
	{

	}

	ResponseMessage::ResponseMessage(const QString &qstring) : Message(qstring)
	{
	}
}
