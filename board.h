#ifndef CFORUM_BOARD_H
#define CFORUM_BOARD_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <list>
#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QList>
#include <string>

using namespace std;
namespace fs = std::experimental::filesystem;

namespace cforum
{
	using CommentList = QList<QObject*>;
	using PostList = QList<QObject*>;

	const QString DELETED_MESSAGE = QString::fromUtf8("Oops, 它被删除了...");
	const QString DATETIME_FORMAT = QString::fromUtf8("yyyyMMddHHmmss");

    class Comment : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(int id MEMBER id CONSTANT)
        Q_PROPERTY(int authorID MEMBER authorID CONSTANT)
        Q_PROPERTY(QString content MEMBER content NOTIFY contentChanged)
		Q_PROPERTY(QString time READ getTimeString NOTIFY contentChanged)
		Q_PROPERTY(bool isDeleted MEMBER isDeleted NOTIFY contentChanged)
    public:
        Comment(const int id, QString content, const int authorID);
        Comment(const fs::path filename);
        Comment(const Comment *oldComment);
        Comment(const Comment &oldComment);
		~Comment();
        int id; // primary_kay in a post, start from 1
		QString content;
		QDateTime time;
        int authorID;
		bool isDeleted;
		void deleteContent();
        void initialize(const Comment *oldComment);
		QString getTimeString() const;
        virtual bool load(const fs::path filename);
        virtual bool save(const fs::path filename) const;
	Q_SIGNALS:
		void contentChanged();
    };

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
		QString title; // without `\n`
		CommentList* getComments();
		int visibleCommentCounter = 0;
        bool post(const QString content, const int userID); // newComment is in heap
		bool canDelete() const;
        bool remove(const int commentID); // commentID < comments->size()
		Comment *getCommentByID(const int commentID);
        bool load(const fs::path path);
        bool save(const fs::path path) const;
        void initialize(const Post *oldPost);
	Q_SIGNALS:
		void commentsChanged();
	private:
		CommentList* comments;
    };

    class Board : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(int id MEMBER id CONSTANT)
        Q_PROPERTY(int moderatorID MEMBER moderatorID CONSTANT)
        Q_PROPERTY(QString name MEMBER name CONSTANT)
    public:
        Board(const int id, const QString name);
        Board(const fs::path path);
        ~Board();
        int id; // primary_kay in a post, start from 1
        QString name;
        PostList* posts;
        int moderatorID = -1;
		Post *getPostByID(const int postID);
        bool post(Post *newPost); // newPost is in heap
        bool remove(const int postID); // postID < posts->size()
        bool isModerator(const int userID) const;
        bool setModerator(const int userID);
        bool removeModerator();
        bool load(const fs::path path);
        bool save(const fs::path path) const;
    };
}

#endif // !CFORUM_BOARD_H
