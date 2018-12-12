#include "message.h"

namespace cforum
{
	Message::Message() : QObject()
	{

	}

	Message::Message(const QString &qstring) : QObject()
	{
		stringstream iss(qstring.toStdString());
		int typeInt;
		iss >> typeInt;
		type = static_cast<MessageType>(typeInt);
		messageString = QString::fromStdString(string((istreambuf_iterator<char>(iss)), {}));
	}
}
