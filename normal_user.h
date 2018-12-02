#ifndef CFORUM_NORMAL_USER_H
#define CFORUM_NORMAL_USER_H

#include "user.h"

namespace cforum
{
	const QString INFO_MESSAGE_NORMAL_USER = QString::fromUtf8("普通用户 ");
	
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
}

#endif // !CFORUM_NORMAL_USER_H
