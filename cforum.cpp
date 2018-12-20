#include "cforum.h"


namespace cforum
{
    CForum::CForum() : QObject()
	{
		initializeDatabase();
	}

    CForum::CForum(const fs::path path) : QObject()
	{
		load(path);
	}

	CForum::~CForum()
	{
        for (QObject *&qit : *boards)
		{
			delete static_cast<Board*>(qit);
		}
		delete boards;
		for (User *&uit : *users)
		{
			delete uit;
		}
		delete users;
	}

	void CForum::initializeDatabase()
	{
		if (boards)
		{
			for (QObject *&qit : *boards)
			{
				delete static_cast<Board*>(qit);
			}
			boards->clear();
		}
		else
		{
			boards = new BoardList;
		}
		if (users)
		{
			for (User *&uit : *users)
			{
				delete uit;
			}
			users->clear();
		}
		else
		{
			users = new UserList;
		}
	}

	NormalUser *CForum::addNormalUser(const QString userName, const QString password)
	{
		if (matchRegular(userName, standardRegular) && matchRegular(password, standardRegular) && !getUserByName(userName))
		{
			User *user = getUserByName(userName);
			if (user)
			{
				return nullptr;
			}
			else
			{
				NormalUser *user = new NormalUser(users->size() + 1, userName, password);
				// 用户ID为正数
				users->push_back(user);
				return user;
			}
		}
		else
		{
			// 用户名或者密码不合法
			return nullptr;
		}
	}

	User * CForum::getUserByName(const QString userName) const
	{
		for (User *user : *users)
		{
			if (user->getName() == userName)
			{
				return user;
			}
		}
		return nullptr;
	}

	User * CForum::login(const QString userName, const QString password)
	{
		User *user = getUserByName(userName);
		if (user && user->login(password))
		{
			return user;
		}
		else
		{
			return nullptr;
		}
	}

	bool CForum::logout(const QString userName)
	{
		User *user = getUserByName(userName);
		if (user)
		{
			user->logout();
			return true;
		}
		else
		{
			return false;
		}
	}

	Board * CForum::addBoard(const QString boardName)
	{
		if (matchRegular(boardName, standardRegular))
		{
			Board *board = new Board(boards->size() + 1, boardName);
			boards->push_back(board);
			return board;
		}
		else
		{
			// 版面名不合法
			return nullptr;
		}
	}

	BoardList * CForum::getBoards() const
	{
		return boards;
	}

	bool CForum::isAdmin(const int userID) const
    {
        return getUserByID(userID)->isAdmin();
    }

	bool CForum::setModerator(const int boardID, const int userID)
	{
		Board *board = getBoardByID(boardID);
		User *user = getUserByID(userID);
		if (board && user)
		{
			if (user->isModerator())
			{
				// 本已是版主
				static_cast<Moderator*>(user)->setModerator(board->getID());
				board->setModerator(user->getID());
				return true;
			}
			else if (!user->isAdmin())
			{
				// user is normal user，升级为版主
				board->setModerator(user->getID());
				for (UserList::iterator qit = users->begin(); qit != users->end(); qit++)
				{
					NormalUser *oldNormalUser = static_cast<NormalUser*>(*qit);
					if (oldNormalUser->getID() == userID)
					{
						Moderator *newModerator = oldNormalUser->toModerator();
						newModerator->setModerator(boardID);
						delete *qit;
						*qit = newModerator;
						break;
					}
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	bool CForum::removeModerator(const int boardID, const int userID)
	{
		Board *board = getBoardByID(boardID);
		User *user = getUserByID(userID);
		if (board && user)
		{
			board->removeModerator(userID);
			Moderator *moderator = static_cast<Moderator*>(user);
			if (moderator->isModerator())
			{
				moderator->removeModerator(boardID);
				if (moderator->getBoardCounter() == 0)
				{
					// 降级为普通用户
					for (UserList::iterator qit = users->begin(); qit != users->end(); qit++)
					{
						Moderator *oldModerator = static_cast<Moderator*>(*qit);
						if (oldModerator->getID() == userID)
						{
							NormalUser *newNormalUser = moderator->toNormalUser();
							delete *qit;
							*qit = newNormalUser;
							break;
						}
					}
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	Post * CForum::addPost(const int boardID, const QString title, const QString content, const int userID)
	{
		Board *board = getBoardByID(boardID);
		User *user = getUserByID(userID);
		if (board && user && !user->isAdmin() && matchRegular(title, inlineRegular))
		{
			Post *post = new Post(board->getPosts()->size() + 1, content, userID, title);
			board->post(post);
			return post;
		}
		else
		{
			// 用户不可发帖，或帖子标题不合法
			return nullptr;
		}
	}

	bool CForum::canRemovePost(const int boardID, const int postID, const int userID) const
	{
		Board *board = getBoardByID(boardID);
		User *user = getUserByID(userID);
		if (board && user && !user->isAdmin()) // 管理员不可删帖
		{
			return board->canRemovePost(postID, userID);
		}
		else
		{
			return false;
		}
	}

	bool CForum::removePost(const int boardID, const int postID, const int userID)
	{
		if (canRemovePost(boardID, postID, userID))
		{
			return getBoardByID(boardID)->remove(postID);
		}
		else
		{
			return false;
		}
	}

	Comment * CForum::addComment(const int boardID, const int postID, const QString content, const int userID)
	{
		Board *board = getBoardByID(boardID);
		User *user = getUserByID(userID);
		if (board && user && !user->isAdmin()) // 管理员不可删帖
		{
			Post *post = board->getPostByID(postID);
			if (post)
			{
				return post->post(content, userID);
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			return nullptr;
		}
	}

	bool CForum::removeComment(const int boardID, const int postID, const int commentID, const int userID)
	{
		Board *board = getBoardByID(boardID);
		User *user = getUserByID(userID);
		if (board && user)
		{
			Post *post = board->getPostByID(postID);
			if (post)
			{
				Comment *comment = post->getCommentByID(commentID);
				if (comment && comment->canRemove() && (user->isAdmin() || user->isModerator(board->getID()) || post->getAuthorID() == userID))
				{
					post->remove(commentID);
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	Board * CForum::getBoardByID(const int boardID) const
	{
        if (boardID <= boards->size())
		{
			BoardList::iterator it = boards->begin();
			advance(it, boardID - 1);
			return static_cast<Board*>(*it);
		}
		else
		{
            return nullptr;
		}
	}

    User * CForum::getUserByID(const int userID) const
    {
        if (userID > 0 && userID <= users->size())
        {
            UserList::iterator it = users->begin();
            advance(it, userID - 1);
            return *it;
        }
        else
        {
            return nullptr;
        }
    }

	bool CForum::load(const fs::path path)
	{
		ifstream stream;
		string raw_string;
		int userCounter = 0;
		users = new UserList;
		boards = new BoardList;
		stream.open(path / "user" / "user.cfdata");
		if (stream.is_open())
		{
			// 读取用户表结构
			stream >> userCounter;
			stream.close();
			for (int userID = 1; userID <= userCounter; userID++)
			{
				// 读取用户信息
				stream.open(path / "user" / (to_string(userID) + ".cfdata"));
				if (stream.is_open())
				{
					int typeInt;
					UserType type;
					User *newUser;
					stream >> typeInt;
					type = static_cast<UserType>(typeInt);
					switch (type)
					{
					case AdminType:
						newUser = new Admin(stream);
						users->push_back(newUser);
						break;
					case GuestType:
						// TODO throw exception
						break;
					case NormalUserType:
						users->push_back(new NormalUser(stream, type));
						break;
					case ModeratorType:
						users->push_back(new Moderator(stream));
						break;
					}
					stream.close();
				}
				else
				{
					return false;
				}
			}
			stream.open(path / "content" / "forum.cfdata");
			if (stream.is_open())
			{
				// 读取版面表结构
				int boardCounter;
				stream >> boardCounter;
				for (int boardID = 1; boardID <= boardCounter; boardID++)
				{
					// 读取版面
					boards->push_back(new Board(path / "content" / to_string(boardID)));
				}
				stream.close();
			}
			else
			{
				return false;
			}
			stream.open(path / "matedata" / "moderator.cfdata");
			if (stream.is_open())
			{
				// 读取版主信息
				getline(stream, raw_string);
				while (raw_string.size() > 1)
				{
					istringstream iss(raw_string);
					int boardID, userID;
					iss >> boardID;
					iss >> userID;
					setModerator(boardID, userID);
					getline(stream, raw_string);
				}
				stream.close();
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	bool CForum::save(const fs::path path) const
	{
		ofstream stream;
		fs::create_directory(path);
		fs::create_directory(path / "user");
		stream.open(path / "user" / "user.cfdata");
		if (stream.is_open())
		{
			// 保存用户表结构
			stream << users->size();
			stream.close();
		}
		else
		{
			return false;
		}
		for (const User *uit : *users)
		{
			// 保存用户信息
			stream.open(path / "user" / (to_string(uit->getID()) + ".cfdata"));
			if (stream.is_open())
			{
				uit->dump(stream);
				stream.close();
			}
			else
			{
				return false;
			}
		}
		fs::create_directory(path / "content");
		stream.open(path / "content" / "forum.cfdata");
		if (stream.is_open())
		{
			stream << boards->size();
			stream.close();
		}
		else
		{
			return false;
		}
		fs::create_directory(path / "matedata");
		stream.open(path / "matedata" / "moderator.cfdata");
		for (QObject *&qit : *boards)
		{
			// 保存版面信息
			Board *bit = static_cast<Board*>(qit);
			bit->save(path / "content" / to_string(bit->getID()));
			if (stream.is_open())
			{
				// 保存版主信息
				bit->saveModerators(stream);
			}
		}
		if (stream.is_open())
		{
			stream.close();
			return true;
		}
		else
		{
			return false;
		}
	}
}
