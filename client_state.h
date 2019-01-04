/**
 * @file client_state.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 定义客户端状态类，处理状态相关的操作。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef CFORUM_CLIENT_STATE_H
#define CFORUM_CLIENT_STATE_H

#include <QObject>

namespace cforum
{
    class ClientState : public QObject
    {
        Q_OBJECT

    public:
        ClientState();
        ClientState(const ClientState &oldClientState);
        ClientState(const ClientState *oldClientState);
        virtual ~ClientState();

        void initialize(const ClientState *oldClientState);

        ClientState operator=(const ClientState &oldClientState);

    public:
        int userID = -1;
        int boardID = -1;
        int postID = -1;
        int commentID = -1;
    };
}

#endif // CFORUM_CLIENT_STATE_H
