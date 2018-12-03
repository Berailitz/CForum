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
			post->deleteContent();
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

    bool Board::load(const fs::path path)
    {
        ifstream stream(path / "board.cfdata");
        posts = new PostList;
        if (stream.is_open())
        {
            string nameTemp;
            int postsCounter;
            stream >> id;
            stream.get(); // 处理行末换行符
			getline(stream, nameTemp);
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
                it->save(path / to_string(it->getID()));
            }
            return true;
        }
        else
        {
            return false;
        }
    }

	void Board::saveModerators(ostream & stream) const
	{
		for (const int moderatorID : *moderators)
		{
			stream << id << " " << moderatorID << endl;
		}
	}
}
