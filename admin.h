#ifndef CFORUM_ADMIN_H
#define CFORUM_ADMIN_H

#include "user.h"

namespace cforum
{
	const QString INFO_MESSAGE_ADMIN = QString::fromUtf8("管理员 ");

	class Admin : public User
	{
        Q_OBJECT
	public:
		Admin(ifstream &stream);
		bool isAdmin() const;
		virtual QString getInfo() const;
	};
}

#endif // !CFORUM_ADMIN_H
