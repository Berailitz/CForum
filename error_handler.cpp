#include "error_handler.h"

namespace cforum
{
    ErrorHandler::ErrorHandler() : QObject()
    {
    }

    void ErrorHandler::raiseError(const QString message)
    {
        emit errorRaised(message);
    }
}

cforum::ErrorHandler *errorHandler = new cforum::ErrorHandler();
