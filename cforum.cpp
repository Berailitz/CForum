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
		NormalUser *user = new NormalUser(users->size() + 1, userName, password);
		users->push_back(user);
		return user;
	}

	User * CForum::getUserByName(const QString userName) const
	{
		for (User *user : *users)
		{
			if (user->userName == userName)
			{
				return user;
			}
		}
		return nullptr;
	}

	User * CForum::checkPassword(const QString userName, const QString password) const
	{
		User *user = getUserByName(userName);
		if (user && user->isPasswordCorrect(password))
		{
			return user;
		}
		else
		{
			return nullptr;
		}
	}

	Board * CForum::addBoard(const QString boardName)
	{
		Board *board = new Board(boards->size() + 1, boardName);
		boards->push_back(board);
		return board;
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
			board->setModerator(user->id);
			if (user->isModerator())
			{
				static_cast<Moderator*>(user)->setModerator(board->id);
			}
			else
			{
				// user is normal user
				for (UserList::iterator qit = users->begin(); qit != users->end(); qit++)
				{
					NormalUser *oldNormalUser = static_cast<NormalUser*>(*qit);
					if (oldNormalUser->id == userID)
					{
						Moderator *newModerator = oldNormalUser->toModerator();
						newModerator->setModerator(boardID);
						delete *qit;
						*qit = newModerator;
					}
				}
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
						if (oldModerator->id == userID)
						{
							NormalUser *newNormalUser = moderator->toNormalUser();
							delete *qit;
							*qit = newNormalUser;
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
		users = new UserList;
		boards = new BoardList;
		admins = new UserSet;
		stream.open(path / "user" / "user.cfdata");
		if (stream.is_open())
		{
			getline(stream, raw_string);
			while (raw_string.size() > 1)
			{
				int typeInt;
				UserType type;
				User *newUser;
				istringstream iss(raw_string);
				iss >> typeInt;
				type = static_cast<UserType>(typeInt);
				switch (type)
				{
				case AdminType:
					newUser = new Admin(iss);
					users->push_back(newUser);
					setAdmin(newUser->id);
					break;
				case GuestType:
					// TODO throw exception
					break;
				case NormalUserType:
					users->push_back(new NormalUser(iss, type));
					break;
				case ModeratorType:
					users->push_back(new Moderator(iss));
					break;
				default:
					break;
				}
				getline(stream, raw_string);
			}
			stream.close();
			for (auto& p : fs::directory_iterator(path / "content"))
			{
				boards->push_back(new Board(p.path()));
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
					getBoardByID(boardID)->setModerator(userID);
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
			for (const User *it : *users)
			{
				stream << it->dump() << endl;
			}
			stream.close();
			fs::create_directory(path / "matedata");
			stream.open(path / "matedata" / "moderator.cfdata");
			fs::create_directory(path / "content");
            for (QObject *&qit: *boards)
            {
                Board *bit = static_cast<Board*>(qit);
				bit->save(path / "content" / to_string(bit->id));
				if (stream.is_open())
				{
					for (const int moderatorID : *bit->moderators)
					{
						stream << bit->id << " " << moderatorID << endl;
					}
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
		else
		{
			return false;
		}
	}
}
