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

    /**
     * @brief 从字符串中加载消息。
     *
     * @param qstring
     */
    void Message::load(const QString & qstring)
    {
        stringstream iss(qstring.toStdString());
        int typeInt;
        iss >> typeInt;
        iss.get();
        type = static_cast<MessageType>(typeInt);
        messageString = QString::fromStdString(
            string((istreambuf_iterator<char>(iss)), {}));
    }

    /**
     * @brief 获取信息的字符串表示，用于在客户端和服务端间传输。
     *
     * @return QString
     */
    QString Message::dump() const
    {
        return QString::number(type) + Q_LINE_BREAK + messageString;
    }

    MessageType Message::getType() const
    {
        return type;
    }

    /**
     * @brief 获取附加信息，即除消息类型外的字符串表示。
     *
     * @return QString
     */
    QString Message::getMessageString() const
    {
        return messageString;
    }

    /**
     * @brief 设置附加信息，即除消息类型外的字符串表示。
     *
     * @param newMessageString
     */
    void Message::setMessageString(const QString & newMessageString)
    {
        messageString = newMessageString;
    }
}
