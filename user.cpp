#include "user.h"

namespace cforum
{
	User::User(const int id, const string userName, const string password) : id(id), userName(userName), password(password)
	{
	}

	User::User(const string text)
	{
		load(text);
	}

	string User::getName() const
	{
		return userName;
	}

	int User::getID() const
	{
		return id;
	}

	bool User::isPasswordCorrect(const string testPassword)
	{
		return password == testPassword;
	}

	string User::dump() const
	{
		return to_string(id) + " " + userName + " " + password;
	}

	bool User::load(const string text)
	{
		istringstream iss(text);
		iss >> id;
		iss >> userName;
		iss >> password;
		return true;
	}
}
