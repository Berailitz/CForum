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
		User(const int id = -1, const QString userName = "", const QString password = "");
		User(const string text);
        User(const User *oldUser);
        User(const User &oldUser);
		int id; // primary_kay, ai, positive integer for typical user
		QString userName; // [A-Za-z0-9_]+
		QString password; // [A-Za-z0-9_]+
		QString getName() const;
		int getID() const;
		bool isPasswordCorrect(const QString testPassword);
		virtual string dump() const = 0;
        bool load(const string text);
        void initialize(const User *oldUser);
	};

	class NormalUser : public User
	{
        Q_OBJECT
	public:
		NormalUser(const int id = -1, const QString userName = "", const QString password = "");
		NormalUser(const string text);
		virtual string dump() const;
	};

	class Admin : public User
	{
        Q_OBJECT
	public:
		virtual string dump() const;
	};

	class Moderator : public NormalUser
	{
        Q_OBJECT
	};

    class Guest : public User
    {
        Q_OBJECT
    public:
        Guest();
		string dump() const;
    };
}

#endif // !CFORUM_USER_H
