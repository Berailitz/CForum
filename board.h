#ifndef CFORUM_BOARD_H
#define CFORUM_BOARD_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <list>
#include <QObject>
#include <string>

using namespace std;
namespace fs = std::experimental::filesystem;

namespace cforum
{
    class Comment : public QObject
    {
        Q_OBJECT
    public:
        Comment(const int id = 0, string content = "", const int authorID = 0);
        Comment(const fs::path filename);
        Comment(const Comment *oldComment);
        Comment(const Comment &oldComment);
        int id; // primary_kay in a thread, start from 1
        string content;
        tm time;
        int authorID;
        void initialize(const Comment *oldComment);
        virtual bool load(const fs::path filename);
        virtual bool save(const fs::path filename) const;
    };


    using CommentList = list<Comment*>;

    class Thread : public Comment
    {
        Q_OBJECT
    public:
        Thread(const int id = 0, string content = "", const int authorID = 0, string title = "");
        Thread(const fs::path path);
        Thread(const Thread *old_thread);
        Thread(const Thread &old_thread);
        virtual ~Thread();
        string title; // without `\n`
        CommentList* comments;
        bool post(Comment *newComment); // newComment is in heap
        bool remove(const int commentID); // commentID < comments->size()
        bool load(const fs::path path);
        bool save(const fs::path path) const;
        void initialize(const Thread *old_thread);
    };


    using ThreadList = list<Thread*>;

    class Board : public QObject
    {
        Q_OBJECT
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
