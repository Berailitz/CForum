#ifndef CFORUM_BOARD_H
#define CFORUM_BOARD_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <list>
#include <QObject>
#include <QList>
#include <string>

using namespace std;
namespace fs = std::experimental::filesystem;
using CommentList = QList<QObject*>;
using ThreadList = QList<QObject*>;

namespace cforum
{
    class Comment : public QObject
    {
        Q_OBJECT
		Q_PROPERTY(int id MEMBER id)
		Q_PROPERTY(int authorID MEMBER authorID)
		Q_PROPERTY(QString content MEMBER content)
    public:
        Comment(const int id = 0, QString content = "", const int authorID = 0);
        Comment(const fs::path filename);
        Comment(const Comment *oldComment);
        Comment(const Comment &oldComment);
        int id; // primary_kay in a thread, start from 1
		QString content;
        tm time;
        int authorID;
        void initialize(const Comment *oldComment);
        virtual bool load(const fs::path filename);
        virtual bool save(const fs::path filename) const;
    };

    class Thread : public Comment
    {
        Q_OBJECT
    public:
        Thread(const int id = 0, QString content = "", const int authorID = 0, QString title = "");
        Thread(const fs::path path);
        Thread(const Thread *old_thread);
        Thread(const Thread &old_thread);
        virtual ~Thread();
		QString title; // without `\n`
        CommentList* comments;
        bool post(Comment *newComment); // newComment is in heap
        bool remove(const int commentID); // commentID < comments->size()
        bool load(const fs::path path);
        bool save(const fs::path path) const;
        void initialize(const Thread *old_thread);
    };

    class Board : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(int id MEMBER id)
        Q_PROPERTY(int moderatorID MEMBER moderatorID)
        Q_PROPERTY(QString name MEMBER name)
    public:
        Board();
        Board(const int id, const QString name);
        Board(const fs::path path);
        ~Board();
        int id; // primary_kay in a thread, start from 1
        QString name;
        ThreadList* threads;
        int moderatorID = -1;
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
