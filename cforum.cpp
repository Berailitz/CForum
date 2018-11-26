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

	Board * CForum::getBoardByID(const int BoardID)
	{
		BoardList::iterator it = boards->begin();
		advance(it, BoardID - 1);
        return static_cast<Board*>(*it);
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
				users->push_back(User(raw_string));
				getline(stream, raw_string);
			}
			stream.close();
			for (auto& p : fs::directory_iterator(path / "content"))
			{
				boards->push_back(new Board(p.path()));
			}
			stream.open(path / "matedata" / "admin.cfdata");
			if (stream.is_open())
			{
				getline(stream, raw_string);
				while (raw_string.size() > 1)
				{
					int adminID = stoi(raw_string);
					admins->insert(adminID);
					getline(stream, raw_string);
				}
				stream.close();
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
			for (const User &it : *users)
			{
				stream << it.dump() << endl;
			}
			stream.close();
			fs::create_directory(path / "matedata");
			stream.open(path / "matedata" / "moderator.cfdata");
			fs::create_directory(path / "content");
            for (QObject *&qit: *boards)
            {
                Board *it = static_cast<Board*>(qit);
				it->save(path / "content" / to_string(it->id));
				if (stream.is_open() && it->moderatorID != -1)
				{
					stream << it->id << " " << it->moderatorID << endl;
				}
			}
			if (stream.is_open())
			{
				stream.close();
			}
			stream.open(path / "matedata" / "admin.cfdata");
			if (stream.is_open())
			{
				for (const int &it : *admins)
				{
					stream << it << endl;
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
}
