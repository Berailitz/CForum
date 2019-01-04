#include "request_message.h"

namespace cforum
{
    RequestMessage::RequestMessage() : Message()
    {

    }

    RequestMessage::RequestMessage(const RequestMessage & oldRequestMessage)
    {
        initialize(&oldRequestMessage);
    }

    RequestMessage::RequestMessage(const RequestMessage * oldRequestMessage)
    {
        initialize(oldRequestMessage);
    }

    RequestMessage::RequestMessage(const QString & qstring) : Message(qstring)
    {
    }

    void RequestMessage::initialize(const RequestMessage * oldRequestMessage)
    {
        type = oldRequestMessage->type;
        clientState = oldRequestMessage->clientState;
        messageString = oldRequestMessage->messageString;
    }

    /**
     * @brief 注册。
     *
     * @param userName
     * @param password
     */
    void RequestMessage::addNormalUser(const QString userName, const QString password)
    {
        type = RegisterRequestMessageType;
        messageString = userName + Q_LINE_BREAK + password + Q_LINE_BREAK;
    }

    /**
     * @brief 登录。
     *
     * @param userName
     * @param password
     */
    void RequestMessage::login(const QString userName, const QString password)
    {
        type = LoginRequestMessageType;
        messageString = userName + Q_LINE_BREAK + password + Q_LINE_BREAK;
    }

    /**
     * @brief 登出。
     *
     */
    void RequestMessage::logout()
    {
        type = LogoutRequestMessageType;
        messageString = "";
    }

    void RequestMessage::addBoard(const QString boardName)
    {
        type = AddBoardMessageType;
        messageString = boardName + Q_LINE_BREAK;
    }

    void RequestMessage::addPost(const int boardID,
        const QString title,
        const QString content,
        const int userID)
    {
        ostringstream out;
        Post post(-1, content, userID, title);
        post.save(out);
        type = AddPostMessageType;
        messageString = QString::number(boardID) + Q_LINE_BREAK;
        messageString += QString::fromStdString(out.str());
    }

    void RequestMessage::removePost(const int boardID,
        const int postID,
        const int userID)
    {
        type = RemovePostMessageType;
        messageString = QString::number(boardID) + Q_LINE_BREAK;
        messageString += QString::number(postID) + Q_LINE_BREAK;
        messageString += QString::number(userID) + Q_LINE_BREAK;
    }

    void RequestMessage::addComment(const int boardID,
        const int postID,
        const QString content,
        const int userID)
    {
        ostringstream out;
        Comment comment(-1, content, userID);
        comment.save(out);
        type = AddCommentMessageType;
        messageString = QString::number(boardID) + Q_LINE_BREAK;
        messageString += (QString::number(postID) + Q_LINE_BREAK);
        messageString += (QString::fromStdString(out.str()));
    }

    void RequestMessage::removeComment(const int boardID,
        const int postID,
        const int commentID,
        const int userID)
    {
        type = RemoveCommentMessageType;
        messageString = QString::number(boardID) + Q_LINE_BREAK;
        messageString += (QString::number(postID) + Q_LINE_BREAK);
        messageString += (QString::number(commentID) + Q_LINE_BREAK);
        messageString += (QString::number(userID) + Q_LINE_BREAK);
    }

    /**
     * @brief 获取版面列表。
     *
     */
    void RequestMessage::getBoardList()
    {
        type = GetBoardListRequestMessageType;
        messageString = "";
    }

    /**
     * @brief 获取主题帖列表。
     *
     * @param boardID
     */
    void RequestMessage::getPostList(const int boardID)
    {
        type = GetPostListRequestMessageType;
        messageString = QString::number(boardID) + Q_LINE_BREAK;
    }

    /**
     * @brief 获取回复帖列表
     *
     * @param boardID
     * @param postID
     */
    void RequestMessage::getCommentList(const int boardID, const int postID)
    {
        type = GetCommentListRequestMessageType;
        messageString = QString::number(boardID) + Q_LINE_BREAK;
        messageString += (QString::number(postID) + Q_LINE_BREAK);
    }

    /**
     * @brief 设置版主
     *
     * @param boardID
     * @param userName
     * @param userID
     */
    void RequestMessage::setModerator(const int boardID,
        const QString userName,
        const int userID)
    {
        type = AddModeratorRequestMessageType;
        messageString = QString::number(boardID) + Q_LINE_BREAK;
        messageString += (userName + Q_LINE_BREAK);
        messageString += (QString::number(userID) + Q_LINE_BREAK);
    }

    /**
     * @brief 撤销版主
     *
     * @param boardID
     * @param userName
     * @param userID
     */
    void RequestMessage::removeModerator(const int boardID,
        const QString userName,
        const int userID)
    {
        type = RemoveModeratorRequestMessageType;
        messageString = QString::number(boardID) + Q_LINE_BREAK;
        messageString += (userName + Q_LINE_BREAK);
        messageString += (QString::number(userID) + Q_LINE_BREAK);
    }
}
