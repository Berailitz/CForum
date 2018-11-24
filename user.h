#ifndef CFORUM_USER
#define CFORUM_USER

#include <sstream>
#include <string>

using namespace std;

namespace cforum
{
	class User
	{
	public:
		User(const int id = -1, const string userName = "", const string password = "");
		User(const string text);
		int id; // primary_kay, ai, positive integer for typical user
		string userName; // [A-Za-z0-9_]+
		string password; // [A-Za-z0-9_]+
		string getName() const;
		int getID() const;
		bool isPasswordCorrect(const string testPassword);
		//string dump() const;
		bool load(const string text);
	};

	class NormalUser : public User
	{
	};

	class Admin : public User
	{
	};

	class Moderator : public NormalUser
	{
	};
}

#endif // !CFORUM_USER
