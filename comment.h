#ifndef CFORUM_COMMENT_H
#define CFORUM_COMMENT_H

#include <filesystem>
#include <fstream>
#include <string>
#include <QDebug>
#include <QObject>
#include <QDateTime>

#include "cfcommon.h"

using namespace std;
namespace fs = std::experimental::filesystem;

namespace cforum
{
    const QString BACK_END_DATETIME_FORMAT = QString::fromUtf8("yyyyMMddHHmmss");
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
        int getID() const;
        QString getContent() const;
        int getAuthorID() const;
        virtual void removeContent();
        void initialize(const Comment *oldComment);
        QString getTimeString() const;
        virtual bool canRemove() const; // 对回复帖而言，未被删除的（回复帖）都能被（某些用户）删除
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
