#include "request_message.h"

namespace cforum
{
	RequestMessage::RequestMessage() : Message()
	{

	}

	void RequestMessage::addNormalUser(const QString userName, const QString password)
	{
		type = RegisterRequestMessageType;
		messageString = userName + LINE_BREAK + password;
	}

	void RequestMessage::login(const QString userName, const QString password)
	{
		type = LoginRequestMessageType;
		messageString = userName + LINE_BREAK + password;
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
		messageString = QString::number(boardID) + LINE_BREAK + QString::number(1) + LINE_BREAK + QString::fromStdString(out.str());
	}

	void RequestMessage::removePost(const int boardID, const int postID, const int userID)
	{
		type = RemovePostMessageType;
		messageString = QString::number(boardID) + LINE_BREAK + QString::number(postID);
	}

	void RequestMessage::addComment(const int boardID, const int postID, const QString content, const int userID)
	{
		ostringstream out;
		Comment comment(-1, content, userID);
		comment.save(out);
		type = AddCommentMessageType;
		messageString = QString::number(boardID) + LINE_BREAK + QString::number(postID) + LINE_BREAK + QString::fromStdString(out.str());
	}

	void RequestMessage::removeComment(const int boardID, const int postID, const int commentID, const int userID)
	{
		type = RemoveCommentMessageType;
		messageString = QString::number(boardID) + LINE_BREAK + QString::number(postID) + LINE_BREAK + QString::number(commentID);
	}

	void RequestMessage::setModerator(const int boardID, const QString userName)
	{
		type = AddModeratorMessageType;
		messageString = QString::number(boardID) + LINE_BREAK + userName;
	}

	void RequestMessage::removeModerator(const int boardID, const QString userName)
	{
		type = RemoveModeratorMessageType;
		messageString = QString::number(boardID) + LINE_BREAK + userName;
	}
}
