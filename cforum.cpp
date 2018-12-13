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
				NormalUser *user = new NormalUser(static_cast<int>(users->size()) + 1, userName, password);
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
		const fs::path userPath = path / "user";
		const fs::path contentPath = path / "content";
		if (fs::exists(path) && fs::exists(userPath) && fs::exists(contentPath))
		{
			string raw_string;
			int userCounter = count_files(userPath);
			int boardCounter = count_files(contentPath);
			initializeDatabase();
			for (int userID = 1; userID <= userCounter; userID++)
			{
				// 读取用户信息
				ifstream userStream(userPath / (to_string(userID) + ".cfdata"));
				if (userStream.is_open())
				{
					int typeInt;
					UserType type;
					User *newUser;
					userStream >> typeInt;
					type = static_cast<UserType>(typeInt);
					switch (type)
					{
					case AdminType:
						newUser = new Admin();
						userStream >> *newUser;
						users->push_back(newUser);
						break;
					case GuestType:
						errorHandler->raiseError(INVALID_USER_TYPE_MESSAGE);
						break;
					case NormalUserType:
						newUser = new NormalUser();
						userStream >> *newUser;
						users->push_back(newUser);
						break;
					case ModeratorType:
						newUser = new Moderator();
						userStream >> *newUser;
						users->push_back(newUser);
						break;
					}
					userStream.close();
				}
				else
				{
					errorHandler->raiseError(DATABASE_ERROR_READING_MESSAGE);
					return false;
				}
			}
			for (int boardID = 1; boardID <= boardCounter; boardID++)
			{
				// 读取版面
				fs::path boardPath = contentPath / to_string(boardID);
				ifstream boardStream(boardPath / "board.cfdata");
				if (boardStream.is_open())
				{
					Board *newBoard = new Board();
					boardStream >> *newBoard;
					boards->push_back(newBoard);
					newBoard->loadPosts(boardPath);
					boardStream.close();
					for (const int userID : *newBoard->getModerators())
					{
						User *user = getUserByID(userID);
						if (user && user->isModerator())
						{
							static_cast<Moderator*>(user)->setModerator(newBoard->getID());
						}
						else
						{
							errorHandler->raiseError(INVALID_MODERATOR_ID_MESSAGE);
							return false;
						}
					}
				}
				else
				{
					errorHandler->raiseError(DATABASE_ERROR_READING_MESSAGE);
					return false;
				}
			}
			// 检查论坛数据有效性
			if (!checkData())
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			errorHandler->raiseError(DATABASE_ERROR_ILLEGAL_FOLDER);
			return false;
		}
	}

	bool CForum::save(const fs::path path) const
	{
		ofstream userStream;
		ofstream boardStream;
		fs::remove_all(path);
		fs::create_directories(path / "user");
		for (const User *uit : *users)
		{
			// 保存用户信息
			userStream.open(path / "user" / (to_string(uit->getID()) + ".cfdata"));
			if (userStream.is_open())
			{
				userStream << *uit;
				userStream.close();
			}
			else
			{
				errorHandler->raiseError(DATABASE_ERROR_WRITING_MESSAGE);
				return false;
			}
		}
		fs::create_directories(path / "content");
		for (QObject *&qit : *boards)
		{
			// 保存版面信息
			Board *bit = static_cast<Board*>(qit);
			fs::path boardPath = path / "content" / to_string(bit->getID());
			fs::create_directories(boardPath);
			boardStream.open(boardPath / "board.cfdata");
			if (boardStream.is_open())
			{
				// 保存帖子信息
				boardStream << *bit;
				bit->savePosts(boardPath);
				boardStream.close();
			}
			else
			{
				errorHandler->raiseError(DATABASE_ERROR_WRITING_MESSAGE);
				return false;
			}
		}
		return true;
	}

	bool CForum::checkData()
	{
		for (QObject *&qit : *boards)
		{
			// 检查版面信息
			Board *bit = static_cast<Board*>(qit);
			// 检查版主信息
			for (int moderatorID : *bit->getModerators())
			{
				User *user = getUserByID(moderatorID);
				if (!user || !user->isModerator())
				{
					errorHandler->raiseError(INVALID_MODERATOR_ID_MESSAGE);
					return false;
				}
			}
			for (QObject *&qit : *bit->getPosts())
			{
				Post *pit = static_cast<Post*>(qit);
				// 检查主题帖作者
				User *postAuthor = getUserByID(pit->getAuthorID());
				if (!postAuthor || postAuthor->isAdmin())
				{
					errorHandler->raiseError(INVALID_POST_AUTHOR_ID_MESSAGE);
					return false;
				}
				// 检查回复帖作者
				for (QObject *&qit : *pit->getComments())
				{
					Comment *cit = static_cast<Comment*>(qit);
					User *commentAuthor = getUserByID(cit->getAuthorID());
					if (!commentAuthor || commentAuthor->isAdmin())
					{
						errorHandler->raiseError(INVALID_COMMENT_AUTHOR_ID_MESSAGE);
						return false;
					}
				}
			}
		}
		return true;
	}

	User * loadUser(istream & userStream)
	{
		User *user;
		int typeInt;
		UserType type;
		userStream >> typeInt;
		type = static_cast<UserType>(typeInt);
		switch (type)
		{
		case AdminType:
			user = new Admin();
			userStream >> *user;
			break;
		case GuestType:
			user = new Guest();
			break;
		case NormalUserType:
			user = new NormalUser();
			userStream >> *user;
			break;
		case ModeratorType:
			user = new Moderator();
			userStream >> *user;
			break;
		}
		return user;
	}
}
