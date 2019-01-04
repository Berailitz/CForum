/**
 * @file moderator.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 定义版主类，实现非管理员的版面管理功能。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef CFORUM_MODERATOR_H
#define CFORUM_MODERATOR_H

#include "normal_user.h"

namespace cforum
{
    const QString INFO_MESSAGE_MODERATOR = QString::fromUtf8("版主 ");

    class Moderator : public NormalUser
    {
        Q_OBJECT

    public:
        Moderator();
        Moderator(const NormalUser &oldNormalUser);
        Moderator(ifstream &stream);

        void setModerator(const int boardID); // 设置为某个版面的版主
        bool removeModerator(const int boardID); // 取消设置为某个版面的版主
        bool isModerator(const int boardID = -1) const; // 检查是否为某个版面的版主

        int getBoardCounter() const; // 是多少个版面的版主
        virtual QString getInfo() const;

        NormalUser *toNormalUser() const; // 降级为普通用户

    private:
        BoardSet *boards = new BoardSet(); // 管理的版面的ID集合
    };
}

#endif // !CFORUM_MODERATOR_H
