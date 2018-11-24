#include "Board.h"


namespace cforum
{
    Board::Board()
    {
    }


    Board::~Board()
    {
    }

    bool Board::post(Thread * newThread)
    {
        threads->push_back(newThread);
        return true;
    }

    bool Board::remove(const int threadID)
    {
        ThreadList::iterator it = threads->begin();
        advance(it, threadID);
        threads->erase(it);
        return true;
    }

    bool Board::isModerator(const int userID) const
    {
        return moderatorID == userID;
    }

    bool Board::setModerator(const int userID)
    {
        moderatorID = userID;
        return true;
    }

    bool Board::removeModerator()
    {
        moderatorID = -1;
        return true;
    }

    Thread::Thread(const int id, string content, const int owner, string title) : Comment(id, content, owner), title(title), comments(new CommentList)
    {
    }

    Thread::Thread(const Thread * old_thread) : Comment(*old_thread)
    {
        initialize(old_thread);
    }

    Thread::Thread(const Thread & old_thread) : Comment(old_thread)
    {
        initialize(&old_thread);
    }

    Thread::~Thread()
    {
        for (Comment* &comment : *comments)
        {
            delete comment;
        }
        delete comments;
    }

    bool Thread::post(Comment * newComment)
    {
        comments->push_back(newComment);
        return true;
    }

    bool Thread::remove(const int commentID)
    {
        CommentList::iterator it = comments->begin();
        advance(it, commentID);
        comments->erase(it);
        return true;
    }

    void Thread::initialize(const Thread * old_thread)
    {
        comments = new CommentList;
        for (Comment* &comment : *old_thread->comments)
        {
            comments->push_back(comment);
        }
    }

    Comment::Comment(const int id, string content, const int owner) : id(id), content(content), owner(owner)
    {
    }
}
