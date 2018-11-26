#include "board.h"


namespace cforum
{
    Board::Board() : QObject(), threads(new ThreadList)
    {
    }

    Board::Board(const int id, const QString name) : QObject(), id(id), name(name), threads(new ThreadList)
    {
    }

    Board::Board(const fs::path path) : QObject()
    {
        load(path);
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
        delete *it;
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

    bool Board::load(const fs::path path)
    {
        ifstream stream(path / "board.cfdata");
        threads = new ThreadList;
        if (stream.is_open())
        {
            string nameTemp;
            int threadsCounter;
            stream >> id;
            stream >> nameTemp;
            name.fromStdString(nameTemp);
            stream >> threadsCounter;
            stream.close();
            for (int i = 1; i <= threadsCounter; i++)
            {
                threads->push_back(new Thread(path / to_string(i)));
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Board::save(const fs::path path) const
    {
        fs::create_directory(path);
        ofstream stream(path / "board.cfdata");
        if (stream.is_open())
        {
            stream << id << endl;
            stream << name.toStdString() << endl;
            stream << threads->size() << endl;
            stream.close();
            for (QObject *&qit : *threads)
            {
                Thread *it = static_cast<Thread*>(qit);
                it->save(path / to_string(it->id));
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    Thread::Thread(const int id, QString content, const int authorID, QString title) : Comment(id, content, authorID), title(title), comments(new CommentList)
    {
    }

    Thread::Thread(const fs::path path)
    {
        load(path);
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
        for (QObject* &qit : *comments)
        {
            Comment *comment = static_cast<Thread*>(qit);
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
        delete *it;
        comments->erase(it);
        return true;
    }

    bool Thread::load(const fs::path path)
    {
        ifstream stream(path / "thread.cfdata");
        comments = new CommentList;
        if (stream.is_open())
        {
			string rawString;
            int commentCounter;
            stream >> id;
            stream >> commentCounter;
            stream.get();
            getline(stream, rawString);
			title = QString::fromStdString(rawString);
            stream >> authorID;
            stream >> get_time(&time, "%Y-%m-%d@%H:%M:%S");
            stream.get();
            content = QString::fromStdString(string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>()));
            stream.close();
            for (int i = 1; i <= commentCounter; i++)
            {
                comments->push_back(new Comment(path / (to_string(i) + ".cfdata")));
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Thread::save(const fs::path path) const
    {
        fs::create_directory(path);
        ofstream stream(path / "thread.cfdata");
        if (stream.is_open())
        {
            stream << id << endl;
            stream << comments->size() << endl;
            stream << title.toStdString() << endl;
            stream << authorID << endl;
            stream << put_time(&time, "%Y-%m-%d@%H:%M:%S") << endl;
            stream << content.toStdString();
            stream.close();
            for (QObject *&qit : *comments)
            {
                Comment *it = static_cast<Thread*>(qit);
                it->save(path / (to_string(it->id) + ".cfdata"));
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    void Thread::initialize(const Thread * old_thread)
    {
        comments = new CommentList;
        for (QObject* &qit : *old_thread->comments)
        {
            Comment *comment = static_cast<Thread*>(qit);
            comments->push_back(comment);
        }
    }

    Comment::Comment(const int id, QString content, const int authorID) : QObject(), id(id), content(content), authorID(authorID)
    {
        auto now = chrono::system_clock::now();
        auto in_time_t = chrono::system_clock::to_time_t(now);
         localtime_s(&time, &in_time_t);
    }

    Comment::Comment(const fs::path filename) : QObject()
    {
        load(filename);
    }

    Comment::Comment(const Comment *oldComment) : QObject()
    {
        initialize(oldComment);
    }

    Comment::Comment(const Comment &oldComment) : QObject()
    {
        initialize(&oldComment);
    }

	void Comment::initialize(const Comment * oldComment)
	{
		id = oldComment->id;
		content = oldComment->content;
		time = oldComment->time;
		authorID = oldComment->authorID;
	}

	bool Comment::load(const fs::path filename)
    {
        ifstream stream(filename);
        if (stream.is_open())
        {
            stream >> id;
            stream >> authorID;
            stream >> get_time(&time, "%Y-%m-%d@%H:%M:%S");
            stream.get();
            content = QString::fromStdString(string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>()));
            stream.close();
            return true;
        }
        else
        {
            return false;
        }
    }
    bool Comment::save(const fs::path filename) const
    {
        ofstream stream(filename);
        if (stream.is_open())
        {
            stream << id << endl;
            stream << authorID << endl;
            stream << put_time(&time, "%Y-%m-%d@%H:%M:%S") << endl;
            stream << content.toStdString();
            stream.close();
            return true;
        }
        else
        {
            return false;
        }
    }
}
