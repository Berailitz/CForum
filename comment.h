#ifndef CFORUM_COMMENT_H
#define CFORUM_COMMENT_H

#include <filesystem>
#include <fstream>
/**
 * @file comment.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 实现回复贴类，表示主题帖下的一条回复。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <QDebug>
#include <QObject>
#include <QDateTime>

#include "cfcommon.h"

using namespace std;
namespace fs = std::experimental::filesystem;

namespace cforum
{
    const QString DELETED_MESSAGE = QString::fromUtf8("Oops, 它被删除了...");

    class Comment : public QObject
    {
        Q_OBJECT
            Q_PROPERTY(int id MEMBER id CONSTANT)
            Q_PROPERTY(int authorID MEMBER authorID CONSTANT)
            Q_PROPERTY(QString content MEMBER content NOTIFY contentChanged)
            Q_PROPERTY(QString time READ getTimeString NOTIFY contentChanged)
            Q_PROPERTY(bool isRemoved MEMBER isRemoved NOTIFY contentChanged)

    public:
        Comment(const int id = -1, QString content = "", const int authorID = -1);
        Comment(const Comment *oldComment);
        Comment(const Comment &oldComment);
        virtual ~Comment();

        void initialize(const Comment *oldComment);

        int getID() const;
        QString getContent() const;
        int getAuthorID() const;
        QString getTimeString() const;

        // 对回复帖而言，未被删除的（回复帖）都能被（某些用户）删除
        virtual bool canRemove() const;
        virtual void removeContent();

        virtual void load(istream &in);
        virtual void save(ostream &out) const;

        friend ostream & operator << (ostream &out, const Comment &comment);
        friend istream & operator >> (istream &in, Comment &comment);

    Q_SIGNALS:
        void contentChanged();

    protected:
        int id; // primary_kay in a post, start from 1
        QString content;
        QDateTime time;
        int authorID;
        bool isRemoved;
    };
}

#endif // !CFORUM_COMMENT_H
