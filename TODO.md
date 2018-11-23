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
    - 论坛主类，继承自`Object`
    - `list<Board*>* boards = nullptr`: 与`ForumView`中的`BoardListView`绑定
    - `vector<User>* users = nullptr`
    - `set<int>* admins = nullptr`
    - `void load(string filename)`

用户
-----

1. `User`类
   - 用户基类，抽象类，继承自`Object`
   - `int id`: primary_kay, ai, positive integer
   - `string userName`: `[A-Za-z0-9_]+`
   - `string password`: `[A-Za-z0-9_]+`
   - `string getName() const`
   - `int getID() const`
   - `virtual bool checkPassword(string test_password)`
   - `string dump() const`: `{name} {password}`
   - `bool load(const string text)`
   - `virtual hello() const`
   - `virtual string getBoardNames() const`
1. `NormalUser`类
   - 继承自`User`类
   - `bool checkPassword(string test_password)`
   - `hello() const`
1. `Moderator`类
   - 继承自`NormalUser`类
   - `bool checkPassword(string test_password)`
   - `hello() const`
   - `string getBoardNames() const`
1. `Admin`类
   - 继承自`User`类
   - `hello() const`
   - `bool checkPassword(string test_password)`

内容
----

1. `Board`类
   - 版面类，继承自`Object`
   - `int id`: primary_kay, 从`1`开始
   - `string name`: unique
   - `list<Thread*>* threads`
   - `int moderators`
   - `bool post(Thread &newThread)`
   - `bool delete(int threadID)`
   - `bool isModerator(int userID)`
1. `Comment`类
   - 回复帖类，继承自`Object`
   - `int ID = -1`: primary_kay in a thread, 从`1`开始
   - `string content`
   - `Datetime time`
   - `int owner`
1. `Thread`类
   - 主题帖类，继承自`Comment`类
   - `int id`: primary_kay in a thread, 从`1`开始
   - `string title`
   - `int owner`
   - `list<Comment*>* comments`
   - `bool post(Comment &newComment)`
   - `bool delete(const int commentID)`

辅助模型
-----

1. `Datetime`类
   - 表示日期和时间
   - `string toString() const`

Controller
=====

1. `ForumController`
    - `public`
        - `CForum cforum* = nullptr`
        - `string username`
        - `string boardName`
        - `string threadTitle`
        - `string thread_content`
        - `bool register(const string userName, const string password)`
        - `bool login(const string userName, const string password)`
        - `bool setModerator(const int userID, const int boardID)`
        - `bool removeModerator(const int userID, const int boardID)`
        - `bool postThread(const string title, const string content)`
        - `bool deleteThread(const int threadID)`
        - `bool postComment(const string content)`
        - `bool deleteComment(const int commentID)`
    - `private`
        - `int _userID = 0`
        - `int _boardID`
        - `int _threadID`

视图
=====

1. `Homepage`
   - 用户名框
   - 密码框
   - 注册按钮
   - 登陆按钮
2. `ForumView`
    - 个人信息
        - 用户名
    - 板块列表: `BoardListView`
        - 板块名
        - 进入按钮
            - `BoardController.load_board(boardID)`
        - 版主管理按钮
            - 版主（列表，计划）
                - 版主用户名
                - 撤销版主按钮
            - 任命版主按钮
2. `BoardView`: `ThreadListView`
    - 板块名
    - 主题帖列表
        - 标题
        - 发帖人
        - 阅读按钮
            - `ThreadController.loadThread(threadID)`
    - 发帖按钮
    - 发帖窗口: `NewThreadPopup`
        - 标题框
        - 正文框
        - 发布按钮
2. `ThreadView`
    - 主题帖标题
    - 主题帖发帖人
    - 主题帖正文
    - 回复帖列表: `CommentListView`
        - 回复正文
        - 回复人用户名
        - 回复时间
    - 回复窗口: `NewCommentPopup`
        - 回复正文
        - 回复按钮
2. `ProfileView`（计划）
    - 个人信息表单
        - 提交按钮

离线数据格式
====

- 用户表
    - 存储至一个文本文件: `/user.cfdata`
    - 每个用户存储至一行文本: `{name} {password}`
- 内容表
    - 存储至一个文件夹: `/content`
    - 每个板块存储至一个子文件夹: `{boardName}`
    - 每个主题帖存储至一个二级子文件夹: `{threadID}`
    - 每个回复贴存储至一个文本文件: `{threadID}`
- 元数据
    - 存储至一个文件夹: `/matedata`
    - 管理员数据存储至一个文本文件`admin.cfdata`，每个管理员存储至一行: `{userID}`
    - 版主数据存储至一个文本文件`moderator.cfdata`，每个版面-版主关系存储至一行: `{boardID} {userID}`

备注
====

1. 用户不能被删除，用户表`id`连续且自增
2. 要手动刷新列表
