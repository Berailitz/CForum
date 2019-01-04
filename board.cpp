#include "board.h"


namespace cforum
{
    Board::Board() : QObject(), posts(new PostList)
    {
    }

    Board::Board(const int id, const QString name)
        : QObject(), id(id), name(name), posts(new PostList)
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

    /**
     * @brief 检查对应主题帖是否存在，存在则返回指向其的指针，否则返回空指针。
     *
     * @param postID
     * @return Post*
     */
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

    /**
     * @brief 检查id是否合法，对应的主题帖能否被该用户删除。
     *
     * @param postID
     * @param userID
     * @return true
     * @return false
     */
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

    /**
     * @brief 若postID合法则直接删除该主题帖，而不检查其能否被删除。
     *
     * @param postID
     * @return true
     * @return false
     */
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

    /**
     * @brief 从版面的版主列表中移除某一用户。
     *
     * @param userID
     * @return true
     * @return false
     */
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
        for (int moderatorID : *moderators)
        {
            out << moderatorID << endl;
        }
    }

    /**
     * @brief 从路径中加载主题帖，如遇异常，报错并返回假。
     *
     * @param boardPath
     * @return true
     * @return false
     */
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

    /**
     * @brief 向路径中保存主题帖，如遇异常，报错并返回假。
     *
     * @param boardPath
     * @return true
     * @return false
     */
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

    /**
     * @brief 不拷贝地返回指向堆区版主列表的指针。
     *
     * @return ModeratorSet*
     */
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
