#include "user.h"

namespace cforum
{
    User::User(const int id, const QString userName, const QString password, UserType type) : QObject(), id(id), userName(userName), password(password), type(type)
	{
	}

    User::User(istringstream &iss, UserType type) : QObject(), type(type)
	{
		load(iss);
	}

    User::User(const User *oldUser) : QObject()
    {
        initialize(oldUser);
    }

    User::User(const User &oldUser) : QObject()
    {
        initialize(&oldUser);
    }

	QString User::getName() const
	{
		return userName;
	}

	int User::getID() const
	{
		return id;
	}

	bool User::isPasswordCorrect(const QString testPassword)
	{
		return password == testPassword;
	}

	bool User::load(istringstream &iss)
	{
		// TODO do NOT load type
		string tempString;
		iss >> id;
		iss >> tempString;
		userName = QString::fromStdString(tempString);
		iss >> tempString;
		password = QString::fromStdString(tempString);
		return true;
	}
	void User::initialize(const User * oldUser)
	{
		id = oldUser->id;
		type = oldUser->type;
		userName = oldUser->userName;
		password = oldUser->password;
	}

	QString User::getGreeting() const
	{
		return WELCOME_MESSAGE_GUEST;
	}

	bool User::isAdmin() const
	{
		return false;
	}

    Guest::Guest() : User(DEFAULT_USER_ID, "Guest", "Guest", GuestType)
    {
    }

	NormalUser::NormalUser(const int id, const QString userName, const QString password, UserType type) : User(id, userName, password, type)
	{
	}

	NormalUser::NormalUser(istringstream &iss, UserType type) : User(iss, type)
	{
	}

	QString NormalUser::getGreeting() const
	{
		return WELCOME_MESSAGE_NORMAL_USER + userName + " !";
	}

	Admin::Admin(istringstream &iss) : User(iss, AdminType)
	{
	}

	QString Admin::getGreeting() const
	{
		return WELCOME_MESSAGE_ADMIN + userName + " !";
	}

	bool Admin::isAdmin() const
	{
		return true;
	}

	Moderator::Moderator(istringstream &iss) : NormalUser(iss, ModeratorType)
	{
	}

	QString Moderator::getGreeting() const
	{
		return WELCOME_MESSAGE_MODERATOR + userName + " !";
	}

	QString Guest::getGreeting() const
	{
		return WELCOME_MESSAGE_GUEST + userName + " !";
	}

	string User::dump() const
	{
		return to_string(type) + " " + to_string(id) + " " + userName.toStdString() + " " + password.toStdString();
	}
}
