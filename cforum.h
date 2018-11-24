#ifndef CFORUM_CFORUM
#define CFORUM_CFORUM

#include <list>
#include <string>
#include <unordered_set>
#include <vector>
#include "board.h"
#include "user.h"

using BoardList = list<cforum::Board*>;
using UserList = vector<cforum::User>;
using AdminSet = unordered_set<int>;

namespace cforum
{
	class CForum
	{
	public:
		CForum();
		~CForum();
		BoardList* boards;
		UserList* users;
		AdminSet* admins;
		bool load(const string path);
	};
}

#endif // !CFORUM_CFORUM
