#ifndef CFORUM_USER_H
#define CFORUM_USER_H

#include <QObject>
#include <sstream>
#include <string>
#include <unordered_set>

using namespace std;

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

	const QString WELCOME_MESSAGE_ADMIN = QString::fromUtf8("欢迎管理员 ");
	const QString WELCOME_MESSAGE_GUEST = QString::fromUtf8("欢迎游客 ");
	const QString WELCOME_MESSAGE_NORMAL_USER = QString::fromUtf8("欢迎普通用户 ");
	const QString WELCOME_MESSAGE_MODERATOR = QString::fromUtf8("欢迎版主 ");

	class User;
	class NormalUser;
	class Admin;
	class Moderator;

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
		virtual bool isAdmin() const;
		virtual bool isModerator(const int boardID = -1) const;
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
		NormalUser(const User *oldUser);
		NormalUser(const int id, const QString userName, const QString password, UserType type = NormalUserType);
		NormalUser(istringstream &iss, UserType type);
		Moderator *toModerator() const;
		virtual QString greeting() const;
	};

	class Admin : public User
	{
        Q_OBJECT
	public:
		Admin(istringstream &iss);
		bool isAdmin() const;
		virtual QString greeting() const;
	};

	class Moderator : public NormalUser
	{
        Q_OBJECT
	public:
		Moderator(const NormalUser &oldNormalUser);
		Moderator(istringstream &iss);
		void setModerator(const int boardID);
		bool removeModerator(const int boardID);
		bool isModerator(const int boardID = -1) const;
		int getBoardCounter() const;
		virtual QString greeting() const;
		NormalUser *toNormalUser() const;
	private:
		BoardSet *boards = new BoardSet();
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
