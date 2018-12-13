#include "request_message.h"

namespace cforum
{
	RequestMessage::RequestMessage() : Message()
	{

	}

	RequestMessage::RequestMessage(const QString & qstring) : Message(qstring)
	{
	}

	void RequestMessage::addNormalUser(const QString userName, const QString password)
	{
		type = RegisterRequestMessageType;
		messageString = userName + Q_LINE_BREAK + password;
	}

	void RequestMessage::login(const QString userName, const QString password)
	{
		type = LoginRequestMessageType;
		messageString = userName + Q_LINE_BREAK + password;
	}

	void RequestMessage::logout()
	{
		type = LogoutRequestMessageType;
		messageString = "";
	}

	void RequestMessage::addBoard(const QString boardName)
	{
		type = AddBoardMessageType;
		messageString = boardName;
	}

	void RequestMessage::addPost(const int boardID, const QString title, const QString content, const int userID)
	{
		ostringstream out;
		Post post(-1, content, userID, title);
		post.save(out);
		type = AddPostMessageType;
		messageString = QString::number(boardID) + Q_LINE_BREAK + QString::fromStdString(out.str());
	}

	void RequestMessage::addComment(const int boardID, const int postID, const QString content, const int userID)
	{
		ostringstream out;
		Comment comment(-1, content, userID);
		comment.save(out);
		type = AddCommentMessageType;
		messageString = QString::number(boardID) + Q_LINE_BREAK + QString::number(postID) + Q_LINE_BREAK + QString::fromStdString(out.str());
	}

	void RequestMessage::setModerator(const int boardID, const QString userName)
	{
		type = AddModeratorMessageType;
		messageString = QString::number(boardID) + Q_LINE_BREAK + userName;
	}

	void RequestMessage::removeModerator(const int boardID, const QString userName)
	{
		type = RemoveModeratorMessageType;
		messageString = QString::number(boardID) + Q_LINE_BREAK + userName;
	}

	void RequestMessage::getBoardList()
	{
		type = GetBoardListRequestMessageType;
		messageString = "";
	}

	void RequestMessage::getPostList(const int boardID)
	{
		type = GetPostListRequestMessageType;
		messageString = QString::number(boardID);
	}

	void RequestMessage::getCommentList(const int boardID, const int postID)
	{
		type = GetCommentListRequestMessageType;
		messageString = QString::number(boardID) + Q_LINE_BREAK + QString::number(postID);
	}
}
