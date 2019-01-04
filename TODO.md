TODO
====

1. 测试
2. WebSocket
3. CS分离

Note
====

1. `QML`中`ListView`对应的容器类型必须为`QList<QObject*>`，元素为`QObject`的子类，使用`Q_OBJECT`标记，并使用`Q_PROPERTY`注册读写函数和信号，无需注册为`QML`类型
2. `QQmlApplicationEngine`加载的`QML`文件需包含`ApplicationWindow`
3. 版面表仅能`append`

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

ClientController
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

- 基本逻辑
    - 客户端为有限状态机，负责展示信息，不含信息处理逻辑
    - 服务端需要知道客户端的状态，如用户`ID`等信息，故服务端需保存各个连接的状态
    - 客户端与服务端采用异步通信，故服务端只需一个线程
    - 服务端的`ForumServer`建立连接，并为每个连接构造一个`ClientDescriptor`实例，并关联其槽函数
    - `Message`
        - 分为请求报文（客户端发给服务端）和响应报文（服务端发给客户端）
    - `ForumServer`
        - 为主函数
        - 为无状态服务
        - 维护数据库，处理业务逻辑
        - 执行`Message`报文，构造对应的响应报文，并单播或广播响应报文
    - `ClientDescriptor`
        - 继承自`ClientState`
        - 依据客户端的状态信息，将客户端发送的文本报文反序列化为`Message`实例
        - 依据客户端的状态信息，检查是否需要发生某个广播的`Message`报文给其对应的客户端
        - 将`Message`实例序列化为文本报文
    - `ClientController`
        - 按需加载数据，即时销毁，不设缓存
    - 凡是需要转方至其他客户端的消息，其中均需包含完整的状态信息，缺省格式为离线数据格式
- 数据结构
    - 状态`class ClientState`
        - `int userID = -1`
        - `int boardID = -1`
        - `int postID = -1`
        - `int commentID = -1`
    - 报文类型`MessageType`
        - `EmptyMessageType`: 空报文
        - `GetBoardListRequestMessageType`: 请求报文，请求版面列表，请求前清空版面列表，服务端依次返回所有版面
        - `GetPostListRequestMessageType`: 请求报文，请求主题帖列表，请求前清空主题帖列表，服务端依次返回所有主题帖
        - `GetCommentListRequestMessageType`: 请求报文，请求回复帖列表，请求前清空回复帖列表，服务端依次返回所有回复帖
        - `RegisterRequestMessageType`: 请求报文，注册
        - `LoginRequestMessageType`: 请求报文，登录
        - `LogoutRequestMessageType`: 请求报文，注销
        - `UpdateUserResponseMessageType`: 响应报文，更新用户信息，仅由服务端发往客户端
        - `ToastResponseMessageType`: 响应报文，显示提示信息
        - `AddBoardMessageType`: 双向，新增版面
        - `AddPostMessageType`: 双向，发主题帖
        - `AddCommentMessageType`: 双向，发回复帖
    - 报文`Message`
        - 基类，表示一个客户端发给服务端，或服务端发给客户端，或服务端中的`Processer`间传递信息的报文
        - 可与`QString`类相互转换
        - `Message(const QString &qstring)`
        - `MessageType type`
        - `ClientState clientState`
        - `void load(const QString &qstring)`: 仅读取第一行，其余部分存于messageString
        - `QString dump() const`
    - 请求报文`class RequestMessage`
        - 继承自`Message`，由客户端发给服务端，用于请求一项操作
        - 客户端执行其`execute`方法
        - 其类型为
            1. `Get*ListRequestMessageType`
            2. `RegisterRequestMessageType`
            2. `LoginRequestMessageType`
            2. `LogoutRequestMessageType`
            1. `AddBoardMessageType`
            1. `AddPostMessageType`
            1. `AddCommentMessageType`
        - `void execute(ForumServer *forumServer) const`: 执行请求，并发出响应
    - 响应报文`class ResponseMessage`
        - 继承自`Message`，由服务端发给客户端，用于通知客户端更新界面和状态
        - 客户端执行其`execute`方法
        - 其类型为
            1. `ToastResponseMessageType`
            2. `UpdateUserResponseMessageType`
            1. `AddBoardMessageType`
            1. `AddPostMessageType`
            1. `AddCommentMessageType`
    - 处理器`ClientDescriptor`
        - 继承自`ClientState`
        - `QWebSocket *client`
    - 客户端控制器`ClientController`
        - 按需加载并保存当前界面上的状态信息
        - `QQmlApplicationEngine *engine = nullptr`
        - `User *user`
        - `Board *board`
        - `Post *post`
        - `void refreshUI()`: 刷新UI信号
        - `void execute(ResponseMessage *message) const`
    - 服务端`ForumServer`
        - `QList<ClientDescriptor*> descriptors`
        - `void boardcast(const Message &message)`
- 程序架构
    1. 采用`WebSocket`长连接，使用文本报文传递信息，双方均可主动发出信息
    2. 报文与的第一行均为报文类型`MessageType`
    3. 服务器异步处理请求，返回响应
    4. 客户端不阻塞UI线程，但禁止任何操作，即将对应的按钮禁用，待收到`refreshUI`信号后恢复
    1. 当其他用户执行操作时，服务器通知各用户
    2. 游客账号的用户名和密码均为`Guest`
    3. 新增主题帖、回复帖的`ID`字段由服务端设置，请求报文中的`ID`字段为`-1`
    4. 主题帖标题由服务端处理，即服务端发送的标题中包含发帖人和时间
    5. 登录成功后，服务端自动下发版面列表
- 协议格式
    - 请求版面列表
        - C: `{GetBoardListRequestMessageType}`
    - 请求主题帖列表
        - C: `{GetPostListRequestMessageType}\n{boardID}`
    - 请求回复帖列表
        - C: `{GetCommentListRequestMessageType}\n{boardID}\n{postID}`
    - 注册
        - C: `{RegisterRequestMessageType}\n{name}{password}`
    - 登录
        - C: `{LoginRequestMessageType}\n{name}{password}`
    - 注销
        - C: `{LogoutRequestMessageType}`
    - 更新用户信息
        - S: `{UpdateUserResponseMessageType}\n{{userID}.cfdata}`
    - 显示提示信息
        - S: `{ToastResponseMessageType}\n{toastText}`
    - 新增版面
        - C: `{AddBoardMessageType}\n{boardName}`
        - S: `{AddBoardMessageType}\n{board.cfdata}`
    - 发主题帖
        - C: `{AddPostMessageType}\n{boardID}\n{{postID}.cfdata}`
        - S: `{AddPostMessageType}\n{boardID}\n{{postID}.cfdata}`
    - 发回复帖
        - C: `{AddCommentMessageType}\n{boardID}\n{postID}\n{{commentID}.cfdata}`
        - S: `{AddCommentMessageType}\n{boardID}\n{postID}\n{{commentID}.cfdata}`
    - 删主题帖
        - C: `{AddPostMessageType}\n{boardID}\n{postID}\n{userID}`
        - S: `{AddPostMessageType}\n{boardID}\n{postID}\n{userID}`
    - 删回复帖
        - C: `{AddCommentMessageType}\n{boardID}\n{postID}\n{commentID}\n{userID}`
        - S: `{AddCommentMessageType}\n{boardID}\n{postID}\n{commentID}\n{userID}`
    - 设置版主
        - C: `{AddModeratorRequestMessageType}\n{boardID}\n{userName}\n{userID}\n`
    - 撤销版主
        - C: `{RemoveModeratorRequestMessageType}\n{boardID}\n{userName}\n{userID}\n`

备注
====

1. 用户不能被删除，用户表`id`从1开始，连续且自增
1. 版面表`id`从1开始，连续且自增
1. 主题帖表`id`从1开始，连续且自增
1. 回复帖表`id`从1开始，连续且自增
2. 要手动刷新列表
