/**
 * @file guest.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 定义游客类，用于表示未登录用户。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef CFORUM_GUEST_H
#define CFORUM_GUEST_H

#include "user.h"

namespace cforum
{
    const QString INFO_MESSAGE_GUEST = QString::fromUtf8("匿名用户");
    const QString GUEST_NAME = QString::fromUtf8("Guest");
    const QString GUEST_PASSWORD = QString::fromUtf8("Guest");

    class Guest : public User
    {
        Q_OBJECT

    public:
        Guest();

        virtual bool isGuest() const;
        QString getInfo() const;
    };
}

#endif // !CFORUM_GUEST_H
