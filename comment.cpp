#include "comment.h"


namespace cforum
{
    Comment::Comment(const int id, QString content, const int authorID) : QObject(), id(id), content(content), authorID(authorID), isRemoved(false)
    {
        time = QDateTime::currentDateTime();
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

    void Comment::removeContent()
    {
        content = DELETED_MESSAGE;
        isRemoved = true;
        emit contentChanged();
    }

    void Comment::initialize(const Comment * oldComment)
    {
        id = oldComment->id;
        content = oldComment->content;
        time = oldComment->time;
        authorID = oldComment->authorID;
        isRemoved = oldComment->isRemoved;
    }

    QString Comment::getTimeString() const
    {
        return time.toString(FRONT_END_DATETIME_FORMAT);
    }

    bool Comment::canRemove() const
    {
        return !isRemoved;
    }

    void Comment::load(istream & in)
    {
        string rawString;
        in >> id;
        in >> authorID;
        in >> isRemoved;
        in.get(); // 处理行末换行符
        getline(in, rawString);
        time = QDateTime::fromString(QString::fromStdString(rawString), BACK_END_DATETIME_FORMAT);
        content = QString::fromStdString(string((istreambuf_iterator<char>(in)), {}));
    }

    void Comment::save(ostream & out) const
    {
        out << id << endl;
        out << authorID << endl;
        out << isRemoved << endl;
        out << time.toString(BACK_END_DATETIME_FORMAT).toStdString() << endl;
        out << content.toStdString();
    }

    ostream & operator<<(ostream & out, const Comment & comment)
    {
        comment.save(out);
        return out;
    }

    istream & operator>>(istream & in, Comment & comment)
    {
        comment.load(in);
        return in;
    }
}
