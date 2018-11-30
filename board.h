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
	using ThreadList = QList<QObject*>;

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
        int id; // primary_kay in a thread, start from 1
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

    class Thread : public Comment
    {
        Q_OBJECT
        Q_PROPERTY(QString title MEMBER title CONSTANT)
		Q_PROPERTY(bool canDelete READ canDelete NOTIFY commentsChanged)
    public:
        Thread(const int id, QString content, const int authorID, QString title);
        Thread(const fs::path path);
        Thread(const Thread *oldThread);
        Thread(const Thread &oldThread);
        virtual ~Thread();
		QString title; // without `\n`
		CommentList* getComments();
		int visibleCommentCounter = 0;
        bool post(const QString content, const int userID); // newComment is in heap
		bool canDelete() const;
        bool remove(const int commentID); // commentID < comments->size()
		Comment *getCommentByID(const int commentID);
        bool load(const fs::path path);
        bool save(const fs::path path) const;
        void initialize(const Thread *oldThread);
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
        int id; // primary_kay in a thread, start from 1
        QString name;
        ThreadList* threads;
        int moderatorID = -1;
		Thread *getThreadByID(const int threadID);
        bool post(Thread *newThread); // newThread is in heap
        bool remove(const int threadID); // threadID < threads->size()
        bool isModerator(const int userID) const;
        bool setModerator(const int userID);
        bool removeModerator();
        bool load(const fs::path path);
        bool save(const fs::path path) const;
    };
}

#endif // !CFORUM_BOARD_H
