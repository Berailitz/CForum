/**
 * @file admin.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 实现管理员类。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef CFORUM_ADMIN_H
#define CFORUM_ADMIN_H

#include "user.h"

namespace cforum
{
    const QString INFO_MESSAGE_ADMIN = QString::fromUtf8("管理员 ");

    class Admin : public User
    {
        Q_OBJECT
    public:
        Admin();
        Admin(ifstream &stream);

        bool isAdmin() const;
        virtual QString getInfo() const;
    };
}

#endif // !CFORUM_ADMIN_H
