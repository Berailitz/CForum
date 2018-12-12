TODO
====

1. 测试
2. WebSocket
3. CS分离

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
    - 每个用户存储至一个文本文件`/user/{id}.cfdata`: `{type}\n{id}\n{lastLoginTime}\n{lastLogoutTime}\n{name}\n{password}`
- 内容表
    - 存储至一个文件夹: `/content`
    - 每个板块存储至一个子文件夹: `/content/{boardID}`，及其下的一个文本文件`board.cfdata`: `{boardID}\n{boardName}\n{postCounter}\n{moderatorID}\n{moderatorID}...`
    - 每个主题帖存储至一个二级子文件夹`/content/{boardID}/{postID}`，及其下的一个文本文件`post.cfdata`: `{postID}\n{commentCounter}\n{isRemoved}\n{authorID}\n{time}\n{title}\n{content}`
    - 每个回复贴存储至一个文本文件`/content/{boardID}/{postID}/{commentID}.cfdata`: `{commentID}\n{authorID}\n{isRemoved}\n{time}\n{content}`

网络访问协议
====

- 基本逻辑：客户端负责显示信息，尽可能不处理信息，信息按需加载，即时销毁，不设缓存
- 数据结构
    - 客户端请求`class Request`
        - 用于通知其他同时在线的用户
        - `RequestType type`
            1. `AddBoardRequestType`
            1. `AddPostRequestType`
            1. `RemovePostRequestType`
            1. `AddCommentRequestType`
            1. `RemoveCommentRequestType`
            1. `AddModeratorRequestType`
            1. `RemoveModeratorRequestType`
        - `int userID = -1`
        - `int boardID = -1`
        - `int postID = -1`
        - `int commentID = -1`
    - 报文类型`MessageType`
        - `RefreshMessageType`: 状态更新
        - `ResponseMessageType`: 服务器的回复，此类报文与客户端发送的前一个报文对应
        - `ToastMessageType`: 需显示在客户端的提示信息
        - `RegisterMessageType`: 注册
        - `LoginMessageType`: 登录
        - `AddBoardMessageType`: 新增版面
        - `GetBoardListMessageType`: 请求版面列表
        - `GetPostListMessageType`: 请求主题帖列表
        - `AddPostMessageType`: 发主题帖
        - `RemovePostMessageType`: 删除主题帖
        - `GetCommentListMessageType`: 请求回复帖列表
        - `AddCommentMessageType`: 发回复帖
        - `RemoveCommentCommentMessageType`: 删除回复帖
        - `AddModeratorMessageType`: 设置版主
        - `RemoveModeratorMessageType`: 取消设置版主
    - 报文`ResponseMessage`
        - `QQmlApplicationEngine &engine`
        - `MessageType type`
        - `void execute() const`
- 程序架构
    1. 采用`WebSocket`长连接，使用文本报文传递信息，双方均可主动发出信息
    2. 每一个报文的第一行均为报文类型`MessageType`
    2. 服务器每收到一个报文，都会返回一个
    2. `ResponseMessageType`报文至少包含`行`，其第二行为一个`int`型状态码，`0`表识成功，非`0`否则失败
    3. 仅有`2`行的`ResponseMessageType`报文，即基本应答报文，其格式为：`{ResponseMessageType}\n{statusCode}`
    3. 服务器返回每个报文后，可以再返回一个报文，即提示信息报文，类型为`ToastMessageType`，以显示提示信息
    2. 每建立一个连接，创建一个`Processer`线程，类似于`Controller`，`Processer`保存对应连接的状态信息，包括
        - `CForum *cforum`
        - `int userID`
        - `int boardID`
        - `int postID`
    1. 当其他用户执行操作时，服务器通知各用户
    2. 客户端即为一个`Controller`，按需加载并保存当前界面上的状态信息，即
        - `User *user`
        - `Board *board`
        - `Post *post`
    2. 客户端每执行一项操作后，即将对应的按钮禁用，待收到`refreshUI`信号后恢复
    2. 游客账号的用户名和密码均为`Guest`
- 协议格式（略去基本应答报文和提示信息报文）
    - 注册
        - C: `{RegisterMessageType}\n{name}{password}`
    - 登录
        - C: `{LoginMessageType}\n{name}{password}`
        - S: `{ResponseMessageType}\n{statusCode}\n{type}\n{id}\n{lastLoginTime}\n{lastLogoutTime}\n{name}\n{password}`
    - 新增版面
        - C: `{AddPostMessageType}\n{boardName}`
    - 请求版面列表
        - C: `{GetBoardListMessageType}`
        - S: `{ResponseMessageType}\n{statusCode}\n{boardName1}\n...`
    - 请求主题帖列表
        - C: `{GetPostListMessageType}\n{boardID}`
        - S: `{ResponseMessageType}\n{statusCode}\n{canRemove}\n{postTitle1}\n...`
    - 发主题帖
        - C: `{AddPostMessageType}\n{title}{content}`
    - 删除主题帖
        - C: `{RemovePostMessageType}\n{postID}`
    - 请求回复帖列表
        - C: `{GetCommentListMessageType}\n{postID}`
        - S: `{ResponseMessageType}\n{statusCode}`
        - S: `{ResponseMessageType}\n{commentID}\n{authorID}\n{isRemoved}\n{time}\n{content}`
        - S: `...`
    - 发回复帖
        - C: `{LoginMessageType}\n{content}`
    - 删除回复帖
        - C: `{LoginMessageType}\n{commentID}`
    - 设置版主
        - C: `{LoginMessageType}\n{userName}`
    - 取消设置版主
        - C: `{LoginMessageType}\n{userName}`
    - 状态更新
        - C: `{RefreshMessageType}\n{userName}`
        - #TODO#

备注
====

1. 用户不能被删除，用户表`id`从1开始，连续且自增
1. 版面表`id`从1开始，连续且自增
1. 主题帖表`id`从1开始，连续且自增
1. 回复帖表`id`从1开始，连续且自增
2. 要手动刷新列表
