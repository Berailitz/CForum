TODO
====

1. 测试
2. 游客不可发帖
3. 转数据库
4. 用户名和版面名查重

Note
====

1. `QML`中`ListView`对应的容器类型必须为`QList<QObject*>`，元素为`QObject`的子类，使用`Q_OBJECT`标记，并使用`Q_PROPERTY`注册读写函数和信号，无需注册为`QML`类型
2. `QQmlApplicationEngine`加载的`QML`文件需包含`ApplicationWindow`

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
    - `list<Board*>* boards = nullptr`: 与`ForumView`中的`BoardListView`绑定
    - `vector<User>* users = nullptr`
    - `unordered_set<int>* admins = nullptr`
    - `bool load(const string filename)`
    - `void save(const string filename) const`

用户
-----

1. `User`类
   - 用户基类，抽象类，继承自`Object`
   - `int id`: primary_kay, ai, positive integer
   - `string userName`: `[A-Za-z0-9_]+`
   - `string password`: `[A-Za-z0-9_]+`
   - `string getName() const`
   - `int getID() const`
   - `virtual bool login(string test_password)`
   - `string dump() const`: `{name} {password}`
   - `bool load(const string text)`
   - `virtual hello() const`
   - `virtual string getBoardNames() const`
1. `NormalUser`类
   - 继承自`User`类
   - `bool login(string test_password)`
   - `hello() const`
1. `Moderator`类
   - 继承自`NormalUser`类
   - `bool login(string test_password)`
   - `hello() const`
   - `string getBoardNames() const`
1. `Admin`类
   - 继承自`User`类
   - `hello() const`
   - `bool login(string test_password)`

内容
----

1. `Board`类
   - 版面类，继承自`Object`
   - `int id`: primary_kay, 从`1`开始
   - `string name`: unique
   - `list<Post*>* posts`
   - `int moderator`
   - `bool post(const Post &newPost)`
   - `bool remove(const int postID)`
   - `bool isModerator(const int userID) const`
   - `bool setModerator(const int userID) const`
   - `bool removeModerator(const int userID) const`
   - `bool load(const string path)`
   - `bool save(const string path) const`
1. `Comment`类
   - 回复帖类，继承自`Object`
   - `int id`: primary_kay in a post, 从`1`开始
   - `string content`
   - `chrono::system_clock::time_point time`
   - `int owner`
   - `virtual bool load(const string path)`
   - `virtual bool save(const string path) const`
1. `Post`类
   - 主题帖类，继承自`Comment`类
   - `string title`: 不含换行符
   - `list<Comment*>* comments`
   - `bool post(const Comment &newComment)`
   - `bool remove(const int commentID)`
   - `bool load(const string path)`
   - `bool save(const string path) const`

Controller
=====

1. `ForumController`
    - `public`
        - `CForum cforum* = nullptr`
        - `string username`
        - `string boardName`
        - `string postTitle`
        - `string postContent`
        - `bool addUser(const string userName, const string password)`
        - `bool login(const string userName, const string password)`
        - `bool setModerator(const int userID, const int boardID)`
        - `bool removeModerator(const int userID, const int boardID)`
        - `bool addPost(const string title, const string content)`
        - `bool removePost(const int postID)`
        - `bool addComment(const string content)`
        - `bool removeComment(const int commentID)`
    - `private`
        - `int _userID = 0`
        - `int _boardID`
        - `int _postID`

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
2. `BoardView`: `PostListView`
    - 板块名
    - 主题帖列表
        - 标题
        - 发帖人
        - 阅读按钮
            - `PostController.loadPost(postID)`
    - 发帖按钮
    - 发帖窗口: `NewPostPopup`
        - 标题框
        - 正文框
        - 发布按钮，发布后进入该主题帖
2. `PostView`
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
    - 表结构存储至一个文本文件`/user/user.cfdata`: `{userCounter}`
    - 每个用户存储至一个文本文件`/user/{id}.cfdata`: `{type}\n{id}\n{lastLoginTime}\n{lastLogoutTime}\n{name}\n{password}`
- 内容表
    - 存储至一个文件夹: `/content`
    - 每个板块存储至一个子文件夹: `/content/{boardID}`，及其下的一个文本文件`board.cfdata`: `{boardID}\n{boardName}\n{postCounter}`
    - 每个主题帖存储至一个二级子文件夹`/content/{boardID}/{postID}`，及其下的一个文本文件`post.cfdata`: `{postID}\n{commentCounter}\n{isDeleted}\n{authorID}\n{time}\n{title}\n{content}`
    - 每个回复贴存储至一个文本文件`/content/{boardID}/{postID}/{commentID}.cfdata`: `{commentID}\n{authorID}\n{isDeleted}\n{time}\n{content}`
- 元数据
    - 存储至一个文件夹: `/matedata`
    - 版主数据存储至一个文本文件`moderator.cfdata`，每个版面-版主关系存储至一行: `{boardID} {userID}`

备注
====

1. 用户不能被删除，用户表`id`从1开始，连续且自增
1. 版面表`id`从1开始，连续且自增
1. 主题帖表`id`从1开始，连续且自增
1. 回复帖表`id`从1开始，连续且自增
2. 要手动刷新列表
