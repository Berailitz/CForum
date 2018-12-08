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
		Board();
		Board(const int id, const QString name);
        ~Board();
		int getID() const;
		QString getName() const;
		PostList* getPosts() const;
		Post *getPostByID(const int postID) const;
        bool post(Post *newPost); // newPost is in heap
		bool canRemovePost(const int postID, const int userID) const; // 普通用户可以删除没有回复的主题帖，版主可以删除任意主题帖
        bool remove(const int postID); // 不检查能否删除，主题帖存在即删除，删帖成功返回真，否则返回假
        bool isModerator(const int userID) const;
        bool setModerator(const int userID);
        bool removeModerator(const int userID);
		void load(istream &in);
		void save(ostream &out) const;
        bool loadPosts(const fs::path boardPath);
		bool savePosts(const fs::path boardPath) const;
		ModeratorSet* getModerators() const;
		friend ostream & operator << (ostream &out, const Board &board);
		friend istream & operator >> (istream &in, Board &board);
	private:
		int id; // primary_kay in a post, start from 1
		QString name;
		PostList* posts;
		ModeratorSet* moderators = new ModeratorSet();
    };
}

#endif // !CFORUM_BOARD_H
