#ifndef CFORUM_USER_H
#define CFORUM_USER_H

#include <QObject>
#include <sstream>
#include <string>

using namespace std;

namespace cforum
{
	enum UserType
	{
		AdminType = -1,
		GuestType,
		NormalUserType,
		ModeratorType
	};
	const int UserTypeMin = -1;
	const int UserTypeMax = 2;

	const QString WELCOME_MESSAGE_ADMIN = QString::fromUtf8("欢迎管理员 ");
	const QString WELCOME_MESSAGE_GUEST = QString::fromUtf8("欢迎游客 ");
	const QString WELCOME_MESSAGE_NORMAL_USER = QString::fromUtf8("欢迎普通用户 ");
	const QString WELCOME_MESSAGE_MODERATOR = QString::fromUtf8("欢迎版主 ");

    class User : public QObject
    {
        Q_OBJECT
	public:
		User(const int id = -1, const QString userName = "", const QString password = "", UserType type = GuestType);
		User(istringstream &iss, UserType type);
        User(const User *oldUser);
        User(const User &oldUser);
		int id; // primary_kay, ai, positive integer for typical user
		UserType type;
		QString userName; // [A-Za-z0-9_]+
		QString password; // [A-Za-z0-9_]+
		QString getName() const;
		int getID() const;
		bool isPasswordCorrect(const QString testPassword);
		virtual QString greeting() const = 0;
		string dump() const;
        bool load(istringstream &iss);
        void initialize(const User *oldUser);
	};

	class NormalUser : public User
	{
        Q_OBJECT
	public:
		NormalUser(const int id, const QString userName, const QString password, UserType type = NormalUserType);
		NormalUser(istringstream &iss, UserType type);
		virtual QString greeting() const;
	};

	class Admin : public User
	{
        Q_OBJECT
	public:
		Admin(istringstream &iss);
		virtual QString greeting() const;
	};

	class Moderator : public NormalUser
	{
        Q_OBJECT
	public:
		Moderator(istringstream &iss);
		virtual QString greeting() const;
	};

    class Guest : public User
    {
        Q_OBJECT
    public:
        Guest();
		QString greeting() const;
    };
}

#endif // !CFORUM_USER_H
