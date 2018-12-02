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
		Q_PROPERTY(bool canDelete READ canDelete NOTIFY commentsChanged)
    public:
        Post(const int id, QString content, const int authorID, QString title);
        Post(const fs::path path);
        Post(const Post *oldPost);
        Post(const Post &oldPost);
        virtual ~Post();
		QString getTitle() const;
		CommentList* getComments();
        bool post(const QString content, const int userID); // newComment is in heap
		bool canDelete() const; // 没有回复、且没有被删除的主题帖可以被删除
        bool remove(const int commentID); // commentID < comments->size()，删除回复贴
		Comment *getCommentByID(const int commentID);
        bool load(const fs::path path);
        bool save(const fs::path path) const;
        void initialize(const Post *oldPost);
	Q_SIGNALS:
		void commentsChanged();
	private:
		int visibleCommentCounter = 0; // 没有被删除的回复贴的数量
		QString title; // without `\n`
		CommentList* comments; // 回复帖列表
    };
}

#endif // !CFORUM_POST_H
