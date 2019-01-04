/**
 * @file user.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 定义用户基类，该类为抽象类，仅实现基本的数据结构。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef CFORUM_USER_H
#define CFORUM_USER_H

#include <filesystem>
#include <fstream>
#include <QDateTime>
#include <QObject>
#include <string>
#include <unordered_set>

#include "cfcommon.h"

using namespace std;
namespace fs = std::experimental::filesystem;

namespace cforum
{
    using BoardSet = unordered_set<int>;
    enum UserType
    {
        AdminType = -1,
        GuestType,
        NormalUserType,
        ModeratorType
    };
    const int UserTypeMin = -1;
    const int UserTypeMax = 2;

    const QString LAST_LOGIN_MESSAGE = QString::fromUtf8("最近登录时间");
    const QString LAST_LOGOUT_MESSAGE = QString::fromUtf8("最近注销时间");

    class User;
    class Moderator;

    class User : public QObject
    {
        Q_OBJECT
    public:
        User(const int id = -1,
            const QString userName = "",
            const QString password = "",
            UserType type = GuestType);
        User(istream &stream, UserType type);
        User(const User *oldUser);
        User(const User &oldUser);
        virtual ~User();

        void initialize(const User *oldUser);

        int getID() const;
        QString getName() const;

        // 登录
        bool login(const QString testPassword);
        // 注销
        void logout();

        virtual bool isAdmin() const;
        virtual bool isGuest() const;
        virtual bool isModerator(const int boardID = -1) const;

        // 获取个人信息，纯虚函数
        virtual QString getInfo() const = 0;

        void dump(ostream &stream) const;
        void load(istream &stream);

        friend ostream & operator << (ostream &out, const User &user);
        friend istream & operator >> (istream &in, User &user);

    protected:
        int id; // 自增主键，普通用户为正数
        UserType type;
        QString userName; // [A-Za-z0-9_]+
        QDateTime lastLoginTime = QDateTime::currentDateTime();
        QDateTime lastLogoutTime = QDateTime::currentDateTime();

        // 获取基本个人信息
        virtual QString getBasicInfo() const;

    private:
        QString password; // [A-Za-z0-9_]+
    };
}

#endif // !CFORUM_USER_H
