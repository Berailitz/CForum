#include "board.h"


namespace cforum
{
    Board::Board(const int id, const QString name) : QObject(), id(id), name(name), threads(new ThreadList)
    {
    }

    Board::Board(const fs::path path) : QObject()
    {
        load(path);
    }


    Board::~Board()
    {
		for (QObject *qit : *threads)
		{
			delete static_cast<Thread*>(qit);
		}
    }

	Thread * Board::getThreadByID(const int threadID)
	{
        if (threadID > 0 && threadID <= threads->size())
		{
			ThreadList::iterator it = threads->begin();
			advance(it, threadID - 1);
			return static_cast<Thread*>(*it);
		}
		else
		{
			return nullptr;
		}
	}

	bool Board::post(Thread * newThread)
    {
        threads->push_back(newThread);
        return true;
    }

    bool Board::remove(const int threadID)
    {
        ThreadList::iterator it = threads->begin();
        advance(it, threadID - 1);
		static_cast<Thread*>(*it)->deleteContent();
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
            name = QString::fromStdString(nameTemp);
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

    Thread::Thread(const fs::path path) : Comment(-1, "", -1)
    {
        load(path);
    }

    Thread::Thread(const Thread * oldThread) : Comment(*oldThread)
    {
        initialize(oldThread);
    }

    Thread::Thread(const Thread & oldThread) : Comment(oldThread)
    {
        initialize(&oldThread);
    }

    Thread::~Thread()
    {
        for (QObject* &qit : *comments)
        {
			Comment *cit;
			cit = static_cast<Comment*>(qit);
            delete cit;
        }
        delete comments;
    }

	CommentList * Thread::getComments()
	{
		return comments;
	}

    bool Thread::post(const QString content, const int userID)
    {
		Comment * newComment = new Comment(comments->size() + 1, content, userID);
        comments->push_back(newComment);
		visibleCommentCounter++;
		emit commentsChanged();
        return true;
    }

	bool Thread::canDelete() const
	{
		return !isDeleted && visibleCommentCounter == 0;
	}

    bool Thread::remove(const int commentID)
    {
        CommentList::iterator it = comments->begin();
        advance(it, commentID - 1);
		static_cast<Comment*>(*it)->deleteContent();
		visibleCommentCounter--;
		emit commentsChanged();
        return true;
    }

	Comment * Thread::getCommentByID(const int commentID)
	{
		if (commentID > 0 && commentID <= comments->size())
		{
			CommentList::iterator it = comments->begin();
			advance(it, commentID - 1);
			return static_cast<Comment*>(*it);
		}
		else
		{
			return nullptr;
		}
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
			stream >> isDeleted;
			stream >> authorID;
			stream.get();
			getline(stream, rawString);
			time = QDateTime::fromString(QString::fromStdString(rawString), DATETIME_FORMAT);
			qDebug() << getTimeString();
            getline(stream, rawString);
			title = QString::fromStdString(rawString);
            content = QString::fromStdString(string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>()));
            stream.close();
            for (int i = 1; i <= commentCounter; i++)
            {
                comments->push_back(new Comment(path / (to_string(i) + ".cfdata")));
				visibleCommentCounter++;
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
			stream << isDeleted << endl;
			stream << authorID << endl;
			stream << time.toString(DATETIME_FORMAT).toStdString() << endl;
            stream << title.toStdString() << endl;
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

    void Thread::initialize(const Thread * oldThread)
    {
		title = oldThread->title;
		visibleCommentCounter = oldThread->visibleCommentCounter;
        comments = new CommentList;
        for (QObject* &qit : *oldThread->comments)
        {
            Comment *comment = static_cast<Thread*>(qit);
            comments->push_back(comment);
        }
    }

    Comment::Comment(const int id, QString content, const int authorID) : QObject(), id(id), content(content), authorID(authorID), isDeleted(false)
    {
		time = QDateTime::currentDateTime();
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

	Comment::~Comment()
	{
	}

	void Comment::deleteContent()
	{
		content = DELETED_MESSAGE;
		isDeleted = true;
		emit contentChanged();
	}

	void Comment::initialize(const Comment * oldComment)
	{
		id = oldComment->id;
		content = oldComment->content;
		time = oldComment->time;
		authorID = oldComment->authorID;
		isDeleted = oldComment->isDeleted;
	}

	QString Comment::getTimeString() const
	{
		return time.toString(QString::fromUtf8("M月d日 H:m:s"));
	}

	bool Comment::load(const fs::path filename)
    {
        ifstream stream(filename);
        if (stream.is_open())
        {
			string rawString;
            stream >> id;
            stream >> authorID;
			stream >> isDeleted;
			stream.get();
			getline(stream, rawString);
			time = QDateTime::fromString(QString::fromStdString(rawString), DATETIME_FORMAT);
			qDebug() << getTimeString();
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
			stream << isDeleted << endl;
            stream << time.toString(DATETIME_FORMAT).toStdString() << endl;
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
