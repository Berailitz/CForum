#include "user.h"

namespace cforum
{
    User::User(const int id, const QString userName, const QString password, UserType type) : QObject(), id(id), userName(userName), password(password), type(type)
    {
    }

    User::User(istream &stream, UserType type) : QObject(), type(type)
    {
        load(stream);
    }

    User::User(const User *oldUser) : QObject()
    {
        initialize(oldUser);
    }

    User::User(const User &oldUser) : QObject()
    {
        initialize(&oldUser);
    }

    User::~User()
    {
    }

    QString User::getName() const
    {
        return userName;
    }

    bool User::isAdmin() const
    {
        return false;
    }

    bool User::isGuest() const
    {
        return false;
    }

    bool User::isModerator(const int boardID) const
    {
        return false;
    }

    int User::getID() const
    {
        return id;
    }

    bool User::login(const QString testPassword)
    {
        if (password == testPassword)
        {
            lastLoginTime = QDateTime::currentDateTime();
            return true;
        }
        else
        {
            return false;
        }

    }

    void User::logout()
    {
        lastLogoutTime = QDateTime::currentDateTime();
    }

    void User::load(istream &stream)
    {
        string tempString;
        stream >> id;
        stream.get(); // 处理行末换行符
        getline(stream, tempString);
        lastLoginTime = QDateTime::fromString(QString::fromStdString(tempString), DATETIME_FORMAT);
        getline(stream, tempString);
        lastLogoutTime = QDateTime::fromString(QString::fromStdString(tempString), DATETIME_FORMAT);
        getline(stream, tempString);
        userName = QString::fromStdString(tempString);
        getline(stream, tempString);
        password = QString::fromStdString(tempString);
    }
    void User::initialize(const User * oldUser)
    {
        id = oldUser->id;
        type = oldUser->type;
        userName = oldUser->userName;
        password = oldUser->password;
    }

    QString User::getBasicInfo() const
    {
        QString text = userName + QString::fromUtf8(" 用户信息：ID: ") + QString::number(id) + QString::fromUtf8("，");
        text += (LAST_LOGIN_MESSAGE + lastLoginTime.toString(FRONT_END_DATETIME_FORMAT));
        text += ("，" + LAST_LOGOUT_MESSAGE + lastLogoutTime.toString(FRONT_END_DATETIME_FORMAT));
        return text;
    }

    void User::dump(ostream &stream) const
    {
        stream << type << endl;
        stream << id << endl;
        stream << lastLoginTime.toString(DATETIME_FORMAT).toStdString() << endl;
        stream << lastLogoutTime.toString(DATETIME_FORMAT).toStdString() << endl;
        stream << userName.toStdString() << endl;
        stream << password.toStdString() << endl;
    }

    ostream & operator<<(ostream & out, const User & user)
    {
        user.dump(out);
        return out;
    }

    istream & operator>>(istream & in, User & user)
    {
        user.load(in);
        return in;
    }
}
