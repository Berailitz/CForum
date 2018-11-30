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

	bool User::isAdmin() const
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

    Guest::Guest() : User(0, "Guest", "Guest", GuestType)
    {
    }

	NormalUser::NormalUser(const User *oldUser) : User(oldUser)
	{
	}

	NormalUser::NormalUser(const int id, const QString userName, const QString password, UserType type) : User(id, userName, password, type)
	{
	}

	NormalUser::NormalUser(istringstream &iss, UserType type) : User(iss, type)
	{
	}

	Moderator * NormalUser::toModerator() const
	{
		return new Moderator(this);
	}

	QString NormalUser::greeting() const
	{
		return WELCOME_MESSAGE_NORMAL_USER + userName + " !";
	}

	Admin::Admin(istringstream &iss) : User(iss, AdminType)
	{
	}

	bool Admin::isAdmin() const
	{
		return true;
	}

	QString Admin::greeting() const
	{
		return WELCOME_MESSAGE_ADMIN + userName + " !";
	}

	Moderator::Moderator(const NormalUser &oldNormalUser) : NormalUser(oldNormalUser)
	{
	}

	Moderator::Moderator(istringstream &iss) : NormalUser(iss, ModeratorType)
	{
	}

	void Moderator::setModerator(const int boardID)
	{
		boards->insert(boardID);
	}

	bool Moderator::removeModerator(const int boardID)
	{
		BoardSet::iterator bit = boards->find(boardID);
		if (bit != boards->end())
		{
			boards->erase(bit);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Moderator::isModerator(const int boardID) const
	{
		if (boardID == -1 || boards->find(boardID) != boards->end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	int Moderator::getBoardCounter() const
	{
		return boards->size();
	}

	QString Moderator::greeting() const
	{
		return WELCOME_MESSAGE_MODERATOR + userName + " !";
	}

	NormalUser * Moderator::toNormalUser() const
	{
		return new NormalUser(static_cast<const User*>(this));
	}

	QString Guest::greeting() const
	{
		return WELCOME_MESSAGE_GUEST + userName + " !";
	}

	string User::dump() const
	{
		return to_string(type) + " " + to_string(id) + " " + userName.toStdString() + " " + password.toStdString();
	}
}
