#include "guest.h"

namespace cforum
{
    Guest::Guest() : User(0, GUEST_NAME, GUEST_PASSWORD, GuestType)
    {
    }

	bool Guest::isGuest() const
	{
		return true;
	}

	QString Guest::getInfo() const
	{
		return INFO_MESSAGE_GUEST;
	}
}
