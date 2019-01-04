#ifndef CFORUM_ERROR_HANDLER_H
#define CFORUM_ERROR_HANDLER_H

/**
 * @file error_handler.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 实现错误处理模块。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

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
