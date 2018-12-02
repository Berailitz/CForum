#include "cforum.h"


namespace cforum
{
    CForum::CForum() : QObject(), boards(new BoardList), users(new UserList), admins(new UserSet)
	{
	}

    CForum::CForum(const fs::path path) : QObject()
	{
		load(path);
	}


	CForum::~CForum()
	{
        for (QObject *&board : *boards)
		{
			delete board;
		}
		delete boards;
		delete users;
		delete admins;
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
				users->push_back(user);
				return user;
			}
		}
		else
		{
			return false;
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
			return nullptr;
		}
	}

	BoardList * CForum::getBoards() const
	{
		return boards;
	}

	bool CForum::isAdmin(const int userID) const
    {
        return admins->find(userID) != admins->end();
    }

	bool CForum::setAdmin(const int userID)
	{
		if (getUserByID(userID))
		{
			admins->insert(userID);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool CForum::setModerator(const int boardID, const int userID)
	{
		Board *board = getBoardByID(boardID);
		User *user = getUserByID(userID);
		if (board && user)
		{
			if (user->isModerator())
			{
				static_cast<Moderator*>(user)->setModerator(board->getID());
				board->setModerator(user->getID());
				return true;
			}
			else if (!user->isAdmin())
			{
				// user is normal user
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
			return nullptr;
		}
	}

	bool CForum::removePost(const int boardID, const int postID, const int userID)
	{
		Board *board = getBoardByID(boardID);
		User *user = getUserByID(userID);
		if (board && user)
		{
			Post *post = board->getPostByID(postID);
			if (post && post->canDelete() && (user->isAdmin() || user->isModerator(board->getID()) || post->getAuthorID() == userID))
			{
				board->remove(postID);
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

	bool CForum::addComment(const int boardID, const int postID, const QString content, const int userID)
	{
		Board *board = getBoardByID(boardID);
		User *user = getUserByID(userID);
		if (board && user && !user->isAdmin())
		{
			Post *post = board->getPostByID(postID);
			if (post)
			{
				post->post(content, userID);
				return true;
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
				if (comment && comment->canDelete() && (user->isAdmin() || user->isModerator(board->getID()) || post->getAuthorID() == userID))
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
        if (userID == 0)
        {
            return guest;
        }
        else if (userID > 0 && userID <= users->size())
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
		admins = new UserSet;
		stream.open(path / "user" / "user.cfdata");
		if (stream.is_open())
		{
			stream >> userCounter;
			stream.close();
			for (int userID = 1; userID <= userCounter; userID++)
			{
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
						setAdmin(newUser->getID());
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
				int boardCounter;
				stream >> boardCounter;
				for (int boardID = 1; boardID <= boardCounter; boardID++)
				{
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
			stream << users->size();
			stream.close();
		}
		else
		{
			return false;
		}
		for (const User *uit : *users)
		{
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
		fs::create_directory(path / "matedata");
		stream.open(path / "matedata" / "moderator.cfdata");
		fs::create_directory(path / "content");
		for (QObject *&qit : *boards)
		{
			Board *bit = static_cast<Board*>(qit);
			bit->save(path / "content" / to_string(bit->getID()));
			if (stream.is_open())
			{
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
