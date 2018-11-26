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
using UserList = vector<cforum::User>;
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
        UserList* users;
        UserSet* admins;
        Board *getBoardByID(const int BoardID);
        bool load(const fs::path path);
        bool save(const fs::path path) const;
    };
}

#endif // !CFORUM_CFORUM_H
