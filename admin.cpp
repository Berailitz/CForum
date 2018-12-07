#include "admin.h"

namespace cforum
{
	Admin::Admin() : User()
	{

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
}
