#include "moderator.h"

namespace cforum
{
	NormalUser::NormalUser() : User()
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

	NormalUser::~NormalUser()
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
}
