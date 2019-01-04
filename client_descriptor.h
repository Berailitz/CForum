/**
 * @file client_descriptor.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 定义客户端描述符，记录客户端的状态信息。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef CFORUM_CLIENT_DESCRIPTOR_H
#define CFORUM_CLIENT_DESCRIPTOR_H

#include <QObject>
#include <QWebSocket>

#include "client_state.h"

namespace cforum
{
    /**
     * @brief 表示一个客户端，其生命周期与对应的连接相同，
     * 其连接的唯一字符串表示在创建时即生成。
     *
     */
    class ClientDescriptor : public ClientState
    {
        Q_OBJECT

    public:
        ClientDescriptor(QWebSocket *socket);
        ~ClientDescriptor();

        QString hash() const;
        void send(const QString &textMessage);
        QWebSocket *getSocket() const;

        friend bool operator==(const ClientDescriptor& lhs, const ClientDescriptor& rhs);

    private:
        QWebSocket *socket;
        QString hashString;
    };

    bool operator==(const ClientDescriptor& lhs, const ClientDescriptor& rhs);

    QString hashSocket(const QWebSocket &socket);
}

#endif // CFORUM_CLIENT_DESCRIPTOR_H
