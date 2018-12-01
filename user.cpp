#include "user.h"

namespace cforum
{
    User::User(const int id, const QString userName, const QString password, UserType type) : QObject(), id(id), userName(userName), password(password), type(type)
	{
	}

    User::User(ifstream &stream, UserType type) : QObject(), type(type)
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

	void User::load(ifstream &stream)
	{
		// TODO do NOT load type
		string tempString;
		stream >> id;
		stream.get();
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

    Guest::Guest() : User(0, "Guest", "Guest", GuestType)
    {
    }

	NormalUser::NormalUser(const User *oldUser) : User(oldUser)
	{
	}

	NormalUser::NormalUser(const int id, const QString userName, const QString password, UserType type) : User(id, userName, password, type)
	{
	}

	NormalUser::NormalUser(ifstream &stream, UserType type) : User(stream, type)
	{
	}

	Moderator * NormalUser::toModerator() const
	{
		return new Moderator(this);
	}

	QString NormalUser::getInfo() const
	{
		return INFO_MESSAGE_NORMAL_USER + getBasicInfo();
	}

	Admin::Admin(ifstream &stream) : User(stream, AdminType)
	{
	}

	bool Admin::isAdmin() const
	{
		return true;
	}

	QString Admin::getInfo() const
	{
		return INFO_MESSAGE_ADMIN + getBasicInfo();
	}

	Moderator::Moderator(const NormalUser &oldNormalUser) : NormalUser(oldNormalUser)
	{
	}

	Moderator::Moderator(ifstream &stream) : NormalUser(stream, ModeratorType)
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

	QString Moderator::getInfo() const
	{
		return INFO_MESSAGE_MODERATOR + getBasicInfo();
	}

	NormalUser * Moderator::toNormalUser() const
	{
		return new NormalUser(static_cast<const User*>(this));
	}

	QString Guest::getInfo() const
	{
		return INFO_MESSAGE_GUEST;
	}

	void User::dump(ofstream &stream) const
	{
		stream << type << endl;
		stream << id << endl;
		stream << lastLoginTime.toString(DATETIME_FORMAT).toStdString() << endl;
		stream << lastLogoutTime.toString(DATETIME_FORMAT).toStdString() << endl;
		stream << userName.toStdString() << endl;
		stream << password.toStdString() << endl;
	}
}
