#ifndef CFORUM_ERROR_HANDLER_H
#define CFORUM_ERROR_HANDLER_H

#include "cfcommon.h"

extern cforum::Controller *forumController;

namespace cforum
{
	void raiseError(const QString message);
}

#endif // CFORUM_ERROR_HANDLER_H
