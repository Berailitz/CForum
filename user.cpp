#include "user.h"

namespace cforum
{
    User::User(const int id, const QString userName, const QString password) : QObject(), id(id), userName(userName), password(password)
	{
	}

    User::User(const string text) : QObject()
	{
		load(text);
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

	string User::dump() const
	{
		return to_string(id) + " " + userName.toStdString() + " " + password.toStdString();
	}

	bool User::load(const string text)
	{
		string tempString;
		istringstream iss(text);
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
		userName = oldUser->userName;
		password = oldUser->password;
	}
}
