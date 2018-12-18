#ifndef CFORUM_CFORUM_H
#define CFORUM_CFORUM_H

#include <list>
#include <QList>
#include <string>
#include <sstream>
#include <unordered_set>
#include <vector>

#include "cfcommon.h"
#include "board.h"
#include "comment.h"
#include "post.h"
#include "user.h"
#include "normal_user.h"
#include "moderator.h"
#include "guest.h"
#include "admin.h"
#include "error_handler.h"


namespace cforum
{
	using BoardList = QList<QObject*>;
	using UserList = vector<cforum::User*>;
	using UserSet = unordered_set<int>;

	const QString standardRegular = "^[ \\w]+$";
	const QString inlineRegular = "^[^\\n]+$";
	const QString INVALID_USER_TYPE_MESSAGE = QString::fromUtf8("用户类型不合法");
	const QString INVALID_MODERATOR_ID_MESSAGE = QString::fromUtf8("版主ID不合法");
	const QString INVALID_POST_AUTHOR_ID_MESSAGE = QString::fromUtf8("主题帖作者ID不合法");
	const QString INVALID_COMMENT_AUTHOR_ID_MESSAGE = QString::fromUtf8("回复帖作者ID不合法");
	const QString DATABASE_ERROR_ILLEGAL_FOLDER = QString::fromUtf8("数据库文件夹结构不合法");
	const QString DATABASE_ERROR_WRITING_MESSAGE = QString::fromUtf8("数据库写入异常");
	const QString DATABASE_ERROR_READING_MESSAGE = QString::fromUtf8("数据库读取异常");

    class CForum : public QObject
    {
        Q_OBJECT
    public:
        CForum();
        CForum(const fs::path path);
        ~CForum();
		void initializeDatabase();
		NormalUser *addNormalUser(const QString userName, const QString password);
		User *getUserByName(const QString userName) const;
		User *login(const QString userName, const QString password);
		bool logout(const QString userName);
		Board *addBoard(const QString boardName);
		BoardList* getBoards() const;
        bool isAdmin(const int userID) const; // 用户不存在则返回假
		bool setModerator(const int boardID, const int userID); // 用户或版面不存在则返回假
		bool removeModerator(const int boardID, const int userID); // 版主或版面不存在则返回假
		Post *addPost(const int boardID, const QString title, const QString content, const int userID);
		bool canRemovePost(const int boardID, const int postID, const int userID) const;
		bool removePost(const int boardID, const int postID, const int userID);
		Comment * addComment(const int boardID, const int postID, const QString content, const int userID);
		bool removeComment(const int boardID, const int postID, const int commentID, const int userID);
        Board *getBoardByID(const int boardID) const;
        User *getUserByID(const int userID) const;
        bool load(const fs::path path);
        bool save(const fs::path path) const;
		bool checkData();
    private:
		BoardList* boards = nullptr;
		UserList* users = nullptr;
    };
}

#endif // !CFORUM_CFORUM_H
