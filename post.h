#ifndef CFORUM_POST_H
#define CFORUM_POST_H

#include <QList>

#include "comment.h"

namespace cforum
{
    using CommentList = QList<QObject*>;

    class Post : public Comment
    {
        Q_OBJECT
            Q_PROPERTY(QString title MEMBER title CONSTANT)
            Q_PROPERTY(bool canRemove READ canRemove NOTIFY commentsChanged)
    public:
        Post(const int id = -1, QString content = "", const int authorID = -1, QString title = "");
        Post(const Post *oldPost);
        Post(const Post &oldPost);
        ~Post();
        QString getTitle() const;
        bool canRemove(const bool isModerator = false) const;
        CommentList* getComments();
        Comment * post(const QString content, const int userID); // newComment is in heap
        void removeContent();
        bool remove(const int commentID); // commentID < comments->size()，删除回复贴
        Comment *getCommentByID(const int commentID);
        void load(istream &in);
        void save(ostream &out) const;
        bool loadComments(const fs::path postPath);
        bool saveComments(const fs::path postPath) const;
        void initialize(const Post *oldPost);
        friend ostream & operator << (ostream &out, const Post &post);
        friend istream & operator >> (istream &in, Post &post);
    Q_SIGNALS:
        void commentsChanged();
    private:
        int visibleCommentCounter = 0; // 没有被删除的回复贴的数量
        QString title; // without `\n`
        CommentList* comments; // 回复帖列表
    };
}

#endif // !CFORUM_POST_H
