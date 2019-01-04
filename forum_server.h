/**
 * @file forum_server.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 实现MVC架构中，服务端的控制器，采用有序资源使用算法避免死锁。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef CFORUM_FORUM_SERVER_H
#define CFORUM_FORUM_SERVER_H

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QNetworkInterface>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtConcurrent>
#include <QWebSocket>
#include <QWebSocketServer>

#include "cforum.h"
#include "request_message.h"
#include "response_message.h"
#include "client_descriptor.h"

namespace cforum
{
    using ClientList = QVector<ClientDescriptor *>;
    using ClientHashList = list<QString>;

    const QString SERVER_START_MESSAGE = QString::fromUtf8("启动服务器，监听的地址如下") +
        Q_LINE_BREAK;
    const QString SERVER_ALREADY_START_MESSAGE = QString::fromUtf8("服务器已启动");
    const QString SERVER_START_ERROR_MESSAGE = QString::fromUtf8("启动服务器失败");
    const QString SERVER_STOP_MESSAGE = QString::fromUtf8("停止服务器");
    const QString SERVER_CONNECTED_MESSAGE = QString::fromUtf8("连接成功");
    const QString SERVER_DISCONNECTED_MESSAGE = QString::fromUtf8("断开连接");
    const QString REGISTER_SUCCESS_MESSAGE = QString::fromUtf8("注册成功");
    const QString ADD_BOARD_SUCCESS_MESSAGE = QString::fromUtf8("添加版面成功");
    const QString ADD_POST_SUCCESS_MESSAGE = QString::fromUtf8("发主题帖成功");
    const QString REMOVE_POST_SUCCESS_MESSAGE = QString::fromUtf8("删主题帖成功");
    const QString REMOVE_POST_ERROR_MESSAGE = QString::fromUtf8("删主题帖失败");
    const QString ADD_COMMENT_SUCCESS_MESSAGE = QString::fromUtf8("发回复帖成功");
    const QString REMOVE_COMMENT_SUCCESS_MESSAGE = QString::fromUtf8("删回复帖成功");
    const QString REMOVE_COMMENT_ERROR_MESSAGE = QString::fromUtf8("删回复帖失败");
    const QString QSTRING_SEPARATOR = QString::fromUtf8(", ");
    const fs::path DEFAULT_DATABASE_FOLDER_PATH = "data";

    class ForumServer : public QWebSocketServer
    {
        Q_OBJECT
    public:
        ForumServer();
        ~ForumServer();

        bool load(const fs::path path);
        bool save() const;
        bool save(const fs::path path) const;

    public Q_SLOTS:
        bool start(const QString portString);
        bool start(const int port = 8118);
        void stop();

        void onErrorRaised(const QString message);
        void onNewConnection();
        void onTextMessageReceived(const QString &textMessage);
        void onDisconnection();
        void sendMessage(QWebSocket &socket, const QString &textMessage);
        void onMessageToSend(const QString &target, const QString &textMessage);

    Q_SIGNALS:
        void messageReceived(const QString &message);
        void messageToSend(QWebSocket &socket, const QString &textMessage);
        void messageToSend(const QString &target, const QString &textMessage);

    private:
        QMutex userMutex;
        QMutex contentMutex;
        QMutex clientsMutex;
        QMutex fileMutex;
        CForum *cforum;
        ClientList *clients;
        fs::path lastPath = DEFAULT_DATABASE_FOLDER_PATH;

        ClientHashList *getAllClientHash();

        void execute(const QString &target, const RequestMessage &request);

        void sendBoard(const QString &target, const Board &board);
        void sendBoardList(const QString &target);
        void broadcastBoard(const Board &board);

        void sendPost(const QString &target, const int boardID, const Post &post);
        void sendPostList(const QString &target, const int boardID);
        void broadcastPost(const int boardID, const Post &post);
        void broadcastPostDeletion(const int boardID, const int postID);

        void sendComment(const QString &target,
            const int boardID,
            const int postID,
            const Comment &comment);
        void sendCommentList(const QString &target, const int boardID, const int postID);
        void broadcastComment(const int boardID, const int postID, const Comment &comment);
        void broadcastCommentDeletion(const int boardID, const int postID, const int commentID);

        void sendToast(const QString &target, const QString &text);
        void addNormalUser(const QString &target, const QString name, const QString password);
        void login(const QString &target, const QString name, const QString password);

        void addBoard(const QString &target, const QString name);
        void addPost(const QString &target, const int boardID, istream &in);

        void removePost(const QString &target,
            const int boardID,
            const int postID,
            const int userID);
        void addComment(const QString &target,
            const int boardID,
            const int postID,
            istream &in);
        void removeComment(const QString &target,
            const int boardID,
            const int postID,
            const int commentID,
            const int userID);
    };
}

#endif // CFORUM_FORUM_SERVER_H
