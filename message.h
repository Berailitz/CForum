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
		EmptyMessageType, // 空报文
		GetBoardListRequestMessageType, // 请求报文，请求版面列表，请求前清空版面列表，服务端依次返回所有版面
		GetPostListRequestMessageType, // 请求报文，请求主题帖列表，请求前清空主题帖列表，服务端依次返回所有主题帖
		GetCommentListRequestMessageType, // 请求报文，请求回复帖列表，请求前清空回复帖列表，服务端依次返回所有回复帖
		RegisterRequestMessageType, // 请求报文，注册
		LoginRequestMessageType, // 请求报文，登录
		LogoutRequestMessageType, // 请求报文，注销
		UpdateUserResponseMessageType, // 响应报文，更新用户信息，仅由服务端发往客户端
		ToastResponseMessageType, // 响应报文，显示提示信息
		AddBoardMessageType, // 双向，新增版面
		AddPostMessageType, // 双向，发主题帖
		AddCommentMessageType, // 双向，发回复帖
	};

	const string STD_LINE_BREAK = "\n";
	const QString Q_LINE_BREAK = QString::fromStdString(STD_LINE_BREAK);

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
