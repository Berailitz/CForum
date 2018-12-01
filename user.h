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

	const QString INFO_MESSAGE_ADMIN = QString::fromUtf8("管理员 ");
	const QString INFO_MESSAGE_GUEST = QString::fromUtf8("游客 ");
	const QString INFO_MESSAGE_NORMAL_USER = QString::fromUtf8("普通用户 ");
	const QString INFO_MESSAGE_MODERATOR = QString::fromUtf8("版主 ");
	const QString LAST_LOGIN_MESSAGE = QString::fromUtf8("上次登录时间");
	const QString LAST_LOGOUT_MESSAGE = QString::fromUtf8("上次注销时间");

	class User;
	class NormalUser;
	class Admin;
	class Moderator;

    class User : public QObject
    {
        Q_OBJECT
	public:
		User(const int id = -1, const QString userName = "", const QString password = "", UserType type = GuestType);
		User(ifstream &stream, UserType type);
        User(const User *oldUser);
        User(const User &oldUser);
		int getID() const;
		QString getName() const;
		bool login(const QString testPassword);
		void logout();
		virtual bool isAdmin() const;
		virtual bool isModerator(const int boardID = -1) const;
		virtual QString getInfo() const = 0;
		void dump(ofstream &stream) const;
        void load(ifstream &stream);
        void initialize(const User *oldUser);
	protected:
		int id; // primary_kay, ai, positive integer for typical user
		UserType type;
		QString userName; // [A-Za-z0-9_]+
		QDateTime lastLoginTime = QDateTime::currentDateTime();
		QDateTime lastLogoutTime = QDateTime::currentDateTime();
		virtual QString getBasicInfo() const;
	private:
		QString password; // [A-Za-z0-9_]+
	};

	class NormalUser : public User
	{
        Q_OBJECT
	public:
		NormalUser(const User *oldUser);
		NormalUser(const int id, const QString userName, const QString password, UserType type = NormalUserType);
		NormalUser(ifstream &stream, UserType type);
		Moderator *toModerator() const;
		virtual QString getInfo() const;
	};

	class Admin : public User
	{
        Q_OBJECT
	public:
		Admin(ifstream &stream);
		bool isAdmin() const;
		virtual QString getInfo() const;
	};

	class Moderator : public NormalUser
	{
        Q_OBJECT
	public:
		Moderator(const NormalUser &oldNormalUser);
		Moderator(ifstream &stream);
		void setModerator(const int boardID);
		bool removeModerator(const int boardID);
		bool isModerator(const int boardID = -1) const;
		int getBoardCounter() const;
		virtual QString getInfo() const;
		NormalUser *toNormalUser() const;
	private:
		BoardSet *boards = new BoardSet();
	};

    class Guest : public User
    {
        Q_OBJECT
    public:
        Guest();
		QString getInfo() const;
    };
}

#endif // !CFORUM_USER_H
