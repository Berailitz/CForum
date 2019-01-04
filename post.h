/**
 * @file post.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 定义主题帖类，其继承自回复帖类，一个主题帖中有若干个回复贴。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

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
        Post(const int id = -1,
            QString content = "",
            const int authorID = -1,
            QString title = "");
        Post(const Post *oldPost);
        Post(const Post &oldPost);
        ~Post();

        void initialize(const Post *oldPost);

        QString getTitle() const;
        CommentList* getComments();

        // 在堆区创建新的回复帖
        Comment * post(const QString content, const int userID);

        void removeContent();
        // 不校验commentID，而删除回复贴
        bool canRemove(const bool isModerator = false) const;
        bool remove(const int commentID);

        Comment *getCommentByID(const int commentID);

        void load(istream &in);
        void save(ostream &out) const;
        bool loadComments(const fs::path postPath);
        bool saveComments(const fs::path postPath) const;


        friend ostream & operator << (ostream &out, const Post &post);
        friend istream & operator >> (istream &in, Post &post);

    Q_SIGNALS:
        void commentsChanged();
    private:
        int visibleCommentCounter = 0; // 没有被删除的回复贴的数量
        QString title; // 单行字符串
        CommentList* comments; // 回复帖列表
    };
}

#endif // !CFORUM_POST_H
