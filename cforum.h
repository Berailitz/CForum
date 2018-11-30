#ifndef CFORUM_CFORUM_H
#define CFORUM_CFORUM_H

#include <list>
#include <QList>
#include <string>
#include <unordered_set>
#include <vector>
#include "board.h"
#include "user.h"

using BoardList = QList<QObject*>;
using UserList = vector<cforum::User*>;
using UserSet = unordered_set<int>;

namespace cforum
{
    class CForum : public QObject
    {
        Q_OBJECT
    public:
        CForum();
        CForum(const fs::path path);
        ~CForum();
        BoardList* boards;
		NormalUser *addNormalUser(const QString userName, const QString password);
		User *getUserByName(const QString userName);
		User *checkPassword(const QString userName, const QString password);
        bool isAdmin(const int userID) const; // 用户不存在则返回假
		bool setAdmin(const int userID); // 用户不存在则返回假
		bool setModerator(const int boardID, const int userID); // 用户或版面不存在则返回假
		bool removeModerator(const int boardID, const int userID); // 版主或版面不存在则返回假
        Board *getBoardByID(const int boardID);
        User *getUserByID(const int userID);
        bool load(const fs::path path);
        bool save(const fs::path path) const;
    private:
		UserSet* admins;
		UserList* users;
        User *guest = new Guest();
    };
}

#endif // !CFORUM_CFORUM_H
