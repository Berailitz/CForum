#include "board.h"


namespace cforum
{
    Post::Post(const int id, QString content, const int authorID, QString title) : Comment(id, content, authorID), title(title), comments(new CommentList)
    {
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

	bool Post::canRemove(const bool isModerator) const
	{
		return Comment::canRemove() && (isModerator || visibleCommentCounter == 0);
	}

	CommentList * Post::getComments()
	{
		return comments;
	}

	Comment *Post::post(const QString content, const int userID)
    {
		Comment * newComment = new Comment(comments->size() + 1, content, userID);
        comments->push_back(newComment);
		visibleCommentCounter++;
		emit commentsChanged();
        return newComment;
    }

	void Post::removeContent()
	{
		Comment::removeContent();
		for (QObject* &qit : *comments)
		{
			Comment *cit;
			cit = static_cast<Comment*>(qit);
			delete cit;
		}
		comments->clear();
		visibleCommentCounter = 0;
	}

    bool Post::remove(const int commentID)
    {
        CommentList::iterator it = comments->begin();
        advance(it, commentID - 1);
		static_cast<Comment*>(*it)->removeContent();
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

	void Post::load(istream &in)
	{
		string rawString;
		int commentCounter;
		in >> id;
		in >> commentCounter;
		in >> isRemoved;
		in >> authorID;
		in.get(); // 处理行末换行符
		getline(in, rawString);
		time = QDateTime::fromString(QString::fromStdString(rawString), BACK_END_DATETIME_FORMAT);
		getline(in, rawString);
		title = QString::fromStdString(rawString);
		content = QString::fromStdString(string(istreambuf_iterator<char>(in), {}));
	}

	void Post::save(ostream & out) const
	{
		out << id << endl;
		out << comments->size() << endl;
		out << isRemoved << endl;
		out << authorID << endl;
		out << time.toString(BACK_END_DATETIME_FORMAT).toStdString() << endl;
		out << title.toStdString() << endl;
		out << content.toStdString();
	}

	bool Post::loadComments(const fs::path postPath)
	{
		visibleCommentCounter = 0;
		int commentsCounter = count_files(postPath) - 1; // 除去post.cfdata
		for (int i = 1; i <= commentsCounter; i++)
		{
			ifstream commentStream(postPath / (to_string(i) + ".cfdata"));
			if (commentStream.is_open())
			{
				Comment *newComment = new Comment();
				commentStream >> *newComment;
				comments->push_back(newComment);
				if (newComment->canRemove())
				{
					visibleCommentCounter++;
				}
				commentStream.close();
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	bool Post::saveComments(const fs::path postPath) const
	{
		fs::create_directories(postPath);
		for (QObject *&qit : *comments)
		{
			Comment *pit = static_cast<Comment*>(qit);
			ofstream commentStream(postPath / (to_string(pit->getID()) + ".cfdata"));
			if (commentStream.is_open())
			{
				commentStream << *pit;
				commentStream.close();
			}
			else
			{
				return false;
			}
		}
		return true;
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

	ostream & operator<<(ostream & out, const Post & post)
	{
		post.save(out);
		return out;
	}

	istream & operator>>(istream & in, Post & post)
	{
		post.load(in);
		return in;
	}
}
