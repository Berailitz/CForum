#ifndef CFORUM_BOARD_H
#define CFORUM_BOARD_H

#include <chrono>
#include <list>
#include <string>

using namespace std;

namespace cforum
{
	class Comment
	{
	public:
		Comment(const int id = 0, string content = "", const int owner = 0);
		int id; // primary_kay in a thread, start from 1
		string content;
		chrono::system_clock::time_point time;
		int owner;
//		bool load(const string path);
		//bool save(const string path) const;
	};


    using CommentList = list<Comment*>;

	class Thread : public Comment
	{
	public:
		Thread(const int id = 0, string content = "", const int owner = 0, string title = "");
		Thread(const Thread *old_thread);
		Thread(const Thread &old_thread);
        virtual ~Thread();
		string title; // without `\n`
		CommentList* comments;
        bool post(Comment *newComment); // newComment is in heap
		bool remove(const int commentID); // commentID < comments->size()
//		virtual bool load(const string path);
		//virtual bool save(const string path) const;
		void initialize(const Thread *old_thread);
	};


    using ThreadList = list<Thread*>;

	class Board
	{
	public:
		Board();
		~Board();
		int id; // primary_kay in a thread, start from 1
		string name;
		ThreadList* threads;
		int moderatorID;
        bool post(Thread *newThread); // newThread is in heap
		bool remove(const int threadID); // threadID < threads->size()
		bool isModerator(const int userID) const;
		bool setModerator(const int userID);
		bool removeModerator();
//		bool load(const string path);
		//bool save(const string path) const;
	};
}

#endif // !CFORUM_BOARD_H
