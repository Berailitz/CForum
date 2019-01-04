#ifndef CFORUM_ERROR_HANDLER_H
#define CFORUM_ERROR_HANDLER_H

#include <QObject>
#include <QString>

namespace cforum
{
    class ErrorHandler : public QObject
    {
        Q_OBJECT
    public:
        ErrorHandler();
        void raiseError(const QString message);
    Q_SIGNALS:
        void errorRaised(const QString message);
    };
}

extern cforum::ErrorHandler *errorHandler;

#endif // CFORUM_ERROR_HANDLER_H
