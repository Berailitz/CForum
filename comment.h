#ifndef CFORUM_COMMENT_H
#define CFORUM_COMMENT_H

#include <filesystem>
#include <fstream>
#include <string>
#include <QDebug>
#include <QObject>
#include <QDateTime>

#include "cfcommon.h"

using namespace std;
namespace fs = std::experimental::filesystem;

namespace cforum
{
	const QString BACK_END_DATETIME_FORMAT = QString::fromUtf8("yyyyMMddHHmmss");
	const QString DELETED_MESSAGE = QString::fromUtf8("Oops, 它被删除了...");

    class Comment : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(int id MEMBER id CONSTANT)
        Q_PROPERTY(int authorID MEMBER authorID CONSTANT)
        Q_PROPERTY(QString content MEMBER content NOTIFY contentChanged)
		Q_PROPERTY(QString time READ getTimeString NOTIFY contentChanged)
		Q_PROPERTY(bool isDeleted MEMBER isDeleted NOTIFY contentChanged)
    public:
        Comment(const int id, QString content, const int authorID);
        Comment(const fs::path filename);
        Comment(const Comment *oldComment);
        Comment(const Comment &oldComment);
		~Comment();
		int getID() const;
		QString getContent() const;
		int getAuthorID() const;
		void deleteContent();
        void initialize(const Comment *oldComment);
		QString getTimeString() const;
		virtual bool canDelete() const;
        virtual bool load(const fs::path filename);
        virtual bool save(const fs::path filename) const;
	Q_SIGNALS:
		void contentChanged();
	protected:
        int id; // primary_kay in a post, start from 1
		QString content;
		QDateTime time;
        int authorID;
		bool isDeleted;
    };
}

#endif // !CFORUM_COMMENT_H
