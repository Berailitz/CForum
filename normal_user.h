/**
 * @file normal_user.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 定义普通类，实现发帖等操作。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef CFORUM_NORMAL_USER_H
#define CFORUM_NORMAL_USER_H

#include "user.h"

namespace cforum
{
    const QString INFO_MESSAGE_NORMAL_USER = QString::fromUtf8("普通用户 ");

    class NormalUser : public User
    {
        Q_OBJECT
    public:
        NormalUser();
        NormalUser(const User *oldUser);
        NormalUser(const int id,
            const QString userName,
            const QString password,
            UserType type = NormalUserType);
        NormalUser(ifstream &stream, UserType type);
        virtual ~NormalUser();

        Moderator *toModerator() const; // 升级为版主

        virtual QString getInfo() const;
    };
}

#endif // !CFORUM_NORMAL_USER_H
