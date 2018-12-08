#include "board.h"


namespace cforum
{
	Board::Board() : QObject(), posts(new PostList)
	{
	}

    Board::Board(const int id, const QString name) : QObject(), id(id), name(name), posts(new PostList)
    {
    }

    Board::~Board()
    {
		for (QObject *qit : *posts)
		{
			delete static_cast<Post*>(qit);
		}
		delete posts;
		delete moderators;
    }

	int Board::getID() const
	{
		return id;
	}

	QString Board::getName() const
	{
		return name;
	}

	PostList * Board::getPosts() const
	{
		return posts;
	}

	Post * Board::getPostByID(const int postID) const
	{
        if (postID > 0 && postID <= posts->size())
		{
			PostList::iterator it = posts->begin();
			advance(it, postID - 1);
			return static_cast<Post*>(*it);
		}
		else
		{
			// 该主题帖不存在
			return nullptr;
		}
	}

	bool Board::post(Post * newPost)
    {
        posts->push_back(newPost);
        return true;
    }

	bool Board::canRemovePost(const int postID, const int userID) const
	{
		Post *post = getPostByID(postID);
		if (post)
		{
			return post->canRemove(isModerator(userID));
		}
		else
		{
			return false;
		}
	}

    bool Board::remove(const int postID)
    {
		Post *post = getPostByID(postID);
		if (post)
		{
			// 存在该主题帖
			post->removeContent();
			return true;
		}
		else
		{
			return false;
		}
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
			// 不存在该版主
			return false;
		}
    }

    void Board::load(istream & in)
    {
		string nameTemp;
		string rawString;
		int postsCounter;
		in >> id;
		in.get(); // 处理行末换行符
		getline(in, nameTemp);
		name = QString::fromStdString(nameTemp);
		in >> postsCounter;
		in.get(); // 处理行末换行符
		// 读取版主信息
		getline(in, rawString);
		while (rawString.size() > 0)
		{
			setModerator(stoi(rawString));
			getline(in, rawString);
		}
    }

	void Board::save(ostream & out) const
	{
		out << id << endl;
		out << name.toStdString() << endl;
		out << posts->size() << endl;
		for (int moderatorID: *moderators)
		{
			out << moderatorID << endl;
		}
	}

	bool Board::loadPosts(const fs::path boardPath)
	{
		int postsCounter = count_files(boardPath) - 1; // 除去post.cfdata
		for (int i = 1; i <= postsCounter; i++)
		{
			fs::path postPath = boardPath / to_string(i);
			ifstream postStream(postPath / "post.cfdata");
			if (postStream.is_open())
			{
				Post *newPost = new Post();
				postStream >> *newPost;
				newPost->loadComments(postPath);
				posts->push_back(newPost);
				postStream.close();
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	bool Board::savePosts(const fs::path boardPath) const
	{
		fs::create_directories(boardPath);
		for (QObject *&qit : *posts)
		{
			Post *pit = static_cast<Post*>(qit);
			fs::path postPath = boardPath / to_string(pit->getID());
			fs::create_directories(postPath);
			ofstream postStream(postPath / "post.cfdata");
			if (postStream.is_open())
			{
				postStream << *pit;
				pit->saveComments(postPath);
				postStream.close();
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	ModeratorSet* Board::getModerators() const
	{
		return moderators;
	}

	ostream & operator<<(ostream & out, const Board & board)
	{
		board.save(out);
		return out;
	}

	istream & operator>>(istream & in, Board & board)
	{
		board.load(in);
		return in;
	}
}
