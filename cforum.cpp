#include "cforum.h"


namespace cforum
{
	CForum::CForum() : boards(new BoardList), users(new UserList), admins(new AdminSet)
	{
	}


	CForum::~CForum()
	{
		for (Board *&board : *boards)
		{
			delete board;
		}
		delete boards;
		delete users;
		delete admins;
	}
}
