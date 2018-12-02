#include "board.h"


namespace cforum
{
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

	QString Post::getTitle() const
	{
		return title;
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
			stream.get(); // 处理行末换行符
			getline(stream, rawString);
			time = QDateTime::fromString(QString::fromStdString(rawString), BACK_END_DATETIME_FORMAT);
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
			stream << time.toString(BACK_END_DATETIME_FORMAT).toStdString() << endl;
            stream << title.toStdString() << endl;
            stream << content.toStdString();
            stream.close();
            for (QObject *&qit : *comments)
            {
                Comment *it = static_cast<Post*>(qit);
                it->save(path / (to_string(it->getID()) + ".cfdata"));
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
}
