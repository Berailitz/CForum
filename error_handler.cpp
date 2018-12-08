#include "error_handler.h"

namespace cforum
{
	void cforum::raiseError(const QString message)
	{
		forumController->raiseError(message);
	}
}
