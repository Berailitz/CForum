需求分析
=====

用户
----

1. 用户的注册、登陆

版面
----
1. 查看版面列表
1. 查看版面内的贴子

帖子
-----

1. 查看一条主题帖
1. 发布主题帖
1. 发布回复帖
1.  删除回复帖
1.  删除主题帖

管理
----

1.  查看用户列表
1. 查看版主列表
1. 任命版主
1. 辞退版主

Model
=====

论坛
----

1. `CForum`类
    - 论坛主类
    - `unordered_map<int, Board>* boards`
    - `unordered_map<int, User>* users`
    - `set<int>* admins`

用户
-----

1. `User`类
   - 用户基类
   - `int id`
   - `string userName`
   - `string password`
   - `string getName() const`
   - `int getID() const`
   - `bool checkPassword(string test_password) const`
1. `NormalUser`类
   - 继承自`User`类
1. `Moderator`类
   - 继承自`NormalUser`类
1. `Admin`类
   - 继承自`User`类

内容
----

1. `Board`类
   - 版面类
   - `int id`
   - `string name`
   - `list<Thread>* threads`
   - `set<int>& moderators`
   - `bool post(Thread &newThread)`
   - `list<Thread>& getThread()`
   - `bool delete(int threadID)`
   - `bool isModerators(int userID)`
1. `Comment`类
   - 回复帖类
   - `int ID = -1`
   - `string content`
   - `Datetime time`
   - `int owner`
1. `Thread`类
   - 主题帖类，继承自`Comment`类
   - `string title`
   - `int owner`
   - `list<Comment>* comments`
   - `bool post(Comment &newComment)`
   - `list<Comment>& getComments()`
   - `bool deleteComment(const int commentID)`

辅助
-----

1. `Datetime`类
   - 表示日期和时间
   - `string toString() const`

Controller
=====

1. `CForumController`
   - `list<Board>::iterator getBoardIterator()`
   - `User* register(const string userName, const string password)`
   - `User* login(const string userName, const string password)`
   - `bool deleteUser(const int userID)`
   - `Board* addBoard(const int userID, const string name)`
   - `Board* deleteBoard(const int userID, const string name)`
   - `bool addModerator(const int userID, const int boardID)`
   - `bool deleteModerator(const int userID, const int boardID)`
1. `BoardController`
   - `list<Thread>::iterator getThreadIterator()`
   - `Thread* post(const int owner, const int boardID, const string title, const string content)`
   - `bool delete(const int owner, const int threadID)`
1. `ThreadController`
   - `list<Comment>::iterator getCommentIterator()`
   - `Comment* post(const int owner, const int threadID, const string title, const string content)`
   - `bool delete(const int owner, const int threadID)`
