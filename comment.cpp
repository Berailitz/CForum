#include "comment.h"


namespace cforum
{
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

	int Comment::getID() const
	{
		return id;
	}

	QString Comment::getContent() const
	{
		return content;
	}

	int Comment::getAuthorID() const
	{
		return authorID;
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
		return time.toString(FRONT_END_DATETIME_FORMAT);
	}

	bool Comment::canDelete() const
	{
		return !isDeleted;
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
			time = QDateTime::fromString(QString::fromStdString(rawString), BACK_END_DATETIME_FORMAT);
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
            stream << time.toString(BACK_END_DATETIME_FORMAT).toStdString() << endl;
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
