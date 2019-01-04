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
    const QString DATETIME_FORMAT = QString::fromUtf8("yyyyMMddHHmmss");

    const QString LAST_LOGIN_MESSAGE = QString::fromUtf8("最近登录时间");
    const QString LAST_LOGOUT_MESSAGE = QString::fromUtf8("最近注销时间");

    class User;
    class Moderator;

    class User : public QObject
    {
        Q_OBJECT
    public:
        User(const int id = -1, const QString userName = "", const QString password = "", UserType type = GuestType);
        User(istream &stream, UserType type);
        User(const User *oldUser);
        User(const User &oldUser);
        virtual ~User();
        int getID() const;
        QString getName() const;
        bool login(const QString testPassword); // 登录
        void logout(); // 注销
        virtual bool isAdmin() const;
        virtual bool isGuest() const;
        virtual bool isModerator(const int boardID = -1) const;
        virtual QString getInfo() const = 0; // 获取个人信息，纯虚函数
        void dump(ostream &stream) const;
        void load(istream &stream);
        void initialize(const User *oldUser);
        friend ostream & operator << (ostream &out, const User &user);
        friend istream & operator >> (istream &in, User &user);
    protected:
        int id; // primary_kay, ai, positive integer for typical user
        UserType type;
        QString userName; // [A-Za-z0-9_]+
        QDateTime lastLoginTime = QDateTime::currentDateTime();
        QDateTime lastLogoutTime = QDateTime::currentDateTime();
        virtual QString getBasicInfo() const; // 获取基本个人信息
    private:
        QString password; // [A-Za-z0-9_]+
    };
}

#endif // !CFORUM_USER_H
