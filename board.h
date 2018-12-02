#ifndef CFORUM_BOARD_H
#define CFORUM_BOARD_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <QDebug>
#include <QObject>
#include <QList>
#include <string>
#include <unordered_set>

#include "cfcommon.h"
#include "post.h"

using namespace std;
namespace fs = std::experimental::filesystem;

namespace cforum
{
	using PostList = QList<QObject*>;
	using ModeratorSet = unordered_set<int>;


	class Comment;
	class Post;

    class Board : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(int id MEMBER id CONSTANT)
        Q_PROPERTY(QString name MEMBER name CONSTANT)
    public:
        Board(const int id, const QString name);
        Board(const fs::path path);
        ~Board();
		int getID() const;
		QString getName() const;
		PostList* getPosts() const;
		Post *getPostByID(const int postID);
        bool post(Post *newPost); // newPost is in heap
        bool remove(const int postID); // postID < posts->size()
        bool isModerator(const int userID) const;
        bool setModerator(const int userID);
        bool removeModerator(const int userID);
        bool load(const fs::path path);
        bool save(const fs::path path) const;
		void saveModerators(ostream &stream) const;
	private:
		int id; // primary_kay in a post, start from 1
		QString name;
		PostList* posts;
		ModeratorSet* moderators = new ModeratorSet();
    };
}

#endif // !CFORUM_BOARD_H
