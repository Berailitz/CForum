#include "message.h"

namespace cforum
{
	Message::Message() : QObject()
	{

	}

	Message::Message(const QString &qstring) : QObject()
	{
		load(qstring);
	}

	void Message::load(const QString & qstring)
	{
		stringstream iss(qstring.toStdString());
		int typeInt;
		iss >> typeInt;
		iss.get();
		type = static_cast<MessageType>(typeInt);
		messageString = QString::fromStdString(string((istreambuf_iterator<char>(iss)), {}));
	}

	QString Message::dump() const
	{
		return QString::number(type) + Q_LINE_BREAK + messageString;
	}

	MessageType Message::getType() const
	{
		return type;
	}
	
	QString Message::getMessageString() const
	{
		return messageString;
	}

	void Message::setMessageString(const QString & newMessageString)
	{
		messageString = newMessageString;
	}
}
