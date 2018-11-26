#ifndef CFORUM_USER_H
#define CFORUM_USER_H

#include <QObject>
#include <sstream>
#include <string>

using namespace std;

namespace cforum
{
    class User : public QObject
    {
        Q_OBJECT
	public:
		User(const int id = -1, const string userName = "", const string password = "");
		User(const string text);
        User(const User *oldUser);
        User(const User &oldUser);
		int id; // primary_kay, ai, positive integer for typical user
		string userName; // [A-Za-z0-9_]+
		string password; // [A-Za-z0-9_]+
		string getName() const;
		int getID() const;
		bool isPasswordCorrect(const string testPassword);
		string dump() const;
        bool load(const string text);
        void initialize(const User *oldUser);
	};

	class NormalUser : public User
	{
        Q_OBJECT
	};

	class Admin : public User
	{
        Q_OBJECT
	};

	class Moderator : public NormalUser
	{
        Q_OBJECT
	};
}

#endif // !CFORUM_USER_H
