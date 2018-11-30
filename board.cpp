#include "board.h"


namespace cforum
{
    Board::Board(const int id, const QString name) : QObject(), id(id), name(name), posts(new PostList)
    {
    }

    Board::Board(const fs::path path) : QObject()
    {
        load(path);
    }


    Board::~Board()
    {
		for (QObject *qit : *posts)
		{
			delete static_cast<Post*>(qit);
		}
    }

	Post * Board::getPostByID(const int postID)
	{
        if (postID > 0 && postID <= posts->size())
		{
			PostList::iterator it = posts->begin();
			advance(it, postID - 1);
			return static_cast<Post*>(*it);
		}
		else
		{
			return nullptr;
		}
	}

	bool Board::post(Post * newPost)
    {
        posts->push_back(newPost);
        return true;
    }

    bool Board::remove(const int postID)
    {
        PostList::iterator it = posts->begin();
        advance(it, postID - 1);
		static_cast<Post*>(*it)->deleteContent();
        return true;
    }

    bool Board::isModerator(const int userID) const
    {
        return moderators->find(userID) != moderators->end();
    }

    bool Board::setModerator(const int userID)
    {
        moderators->insert(userID);
        return true;
    }

    bool Board::removeModerator(const int userID)
    {
		ModeratorSet::iterator mit = moderators->find(userID);
		if (mit != moderators->end())
		{
			moderators->erase(mit);
			return true;
		}
		else
		{
			return false;
		}
    }

    bool Board::load(const fs::path path)
    {
        ifstream stream(path / "board.cfdata");
        posts = new PostList;
        if (stream.is_open())
        {
            string nameTemp;
            int postsCounter;
            stream >> id;
            stream >> nameTemp;
            name = QString::fromStdString(nameTemp);
            stream >> postsCounter;
            stream.close();
            for (int i = 1; i <= postsCounter; i++)
            {
                posts->push_back(new Post(path / to_string(i)));
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
            stream << posts->size() << endl;
            stream.close();
            for (QObject *&qit : *posts)
            {
                Post *it = static_cast<Post*>(qit);
                it->save(path / to_string(it->id));
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    Post::Post(const int id, QString content, const int authorID, QString title) : Comment(id, content, authorID), title(title), comments(new CommentList)
    {
    }

    Post::Post(const fs::path path) : Comment(-1, "", -1)
    {
        load(path);
    }

    Post::Post(const Post * oldPost) : Comment(*oldPost)
    {
        initialize(oldPost);
    }

    Post::Post(const Post & oldPost) : Comment(oldPost)
    {
        initialize(&oldPost);
    }

    Post::~Post()
    {
        for (QObject* &qit : *comments)
        {
			Comment *cit;
			cit = static_cast<Comment*>(qit);
            delete cit;
        }
        delete comments;
    }

	CommentList * Post::getComments()
	{
		return comments;
	}

    bool Post::post(const QString content, const int userID)
    {
		Comment * newComment = new Comment(comments->size() + 1, content, userID);
        comments->push_back(newComment);
		visibleCommentCounter++;
		emit commentsChanged();
        return true;
    }

	bool Post::canDelete() const
	{
		return !isDeleted && visibleCommentCounter == 0;
	}

    bool Post::remove(const int commentID)
    {
        CommentList::iterator it = comments->begin();
        advance(it, commentID - 1);
		static_cast<Comment*>(*it)->deleteContent();
		visibleCommentCounter--;
		emit commentsChanged();
        return true;
    }

	Comment * Post::getCommentByID(const int commentID)
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

    bool Post::load(const fs::path path)
    {
        ifstream stream(path / "post.cfdata");
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

    bool Post::save(const fs::path path) const
    {
        fs::create_directory(path);
        ofstream stream(path / "post.cfdata");
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
                Comment *it = static_cast<Post*>(qit);
                it->save(path / (to_string(it->id) + ".cfdata"));
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    void Post::initialize(const Post * oldPost)
    {
		title = oldPost->title;
		visibleCommentCounter = oldPost->visibleCommentCounter;
        comments = new CommentList;
        for (QObject* &qit : *oldPost->comments)
        {
            Comment *comment = static_cast<Post*>(qit);
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
