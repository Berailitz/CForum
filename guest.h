#ifndef CFORUM_GUEST_H
#define CFORUM_GUEST_H

#include "user.h"

namespace cforum
{
	const QString INFO_MESSAGE_GUEST = QString::fromUtf8("匿名用户");
	
    class Guest : public User
    {
        Q_OBJECT
    public:
        Guest();
		virtual bool isGuest() const;
		QString getInfo() const;
    };
}

#endif // !CFORUM_GUEST_H
