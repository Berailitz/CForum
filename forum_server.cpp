#include "forum_server.h"

namespace cforum
{
	ForumServer::ForumServer() :
		QWebSocketServer(QStringLiteral("WS Server"),
			QWebSocketServer::NonSecureMode),
		cforum(new CForum()),
		clients(new QVector<ClientDescriptor *>)
    {
		connect(this, qOverload<QWebSocket &, const QString &>(&ForumServer::messageToSend),
			this, qOverload<QWebSocket &, const QString &>(&ForumServer::onMessageToSend));
		connect(this, qOverload<const QString &, const QString &>(&ForumServer::messageToSend),
			this, qOverload<const QString &, const QString &>(&ForumServer::onMessageToSend));
    }

	ForumServer::~ForumServer()
	{
		delete cforum;
		for (ClientDescriptor * client : *clients)
		{
			client->getSocket()->close();
			client->deleteLater();
		}
		delete clients;
	}

	bool ForumServer::listen(const int port)
	{
		if (QWebSocketServer::listen(QHostAddress::Any, port))
		{
			emit messageReceived("Listening on " + QString::number(port) + "\n");
			connect(this, &QWebSocketServer::newConnection,
				this, &ForumServer::onNewConnection);
			return true;
		}
		else
		{
			emit messageReceived("CANNOT listening on " + QString::number(port) + "\n");
			return false;
		}
	}

	bool ForumServer::load(const fs::path path)
	{
		return cforum->load(path);
	}

	bool ForumServer::save(const fs::path path) const
	{
		return cforum->save(path);
	}

    void ForumServer::onNewConnection()
    {
		QWebSocket *socket = QWebSocketServer::nextPendingConnection();
		ClientDescriptor *newClient = new ClientDescriptor(socket);
        emit messageReceived(newClient->hash() + " connected.\n");

        connect(socket, &QWebSocket::textMessageReceived,
                this, &ForumServer::onTextMessageReceived);
        connect(socket, &QWebSocket::disconnected,
                this, &ForumServer::onDisconnection);
        *clients << newClient;
    }

    void ForumServer::onTextMessageReceived(const QString &textMessage)
    {
        QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
		RequestMessage message(textMessage);
		emit messageReceived(hashSocket(*socket) + QString::fromUtf8(" > ") + textMessage + "\n");
		QtConcurrent::run(this, &ForumServer::execute, hashSocket(*socket), message);
		//execute(*socket, message);
    }

    void ForumServer::onDisconnection()
    {
        QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
		for (auto *cit = clients->begin(); cit != clients->end(); cit++)
		{
			if ((*cit)->hash() == hashSocket(*socket))
			{
				clients->erase(cit);
				break;
			}
		}
        emit messageReceived(hashSocket(*socket) + " disconnected.\n");
    }

	void ForumServer::onMessageToSend(QWebSocket & socket, const QString & textMessage)
	{
		socket.sendTextMessage(textMessage);
		emit messageReceived(hashSocket(socket) + " < " + textMessage + "\n");
	}

	void ForumServer::onMessageToSend(const QString & target, const QString & textMessage)
	{
		for (ClientDescriptor *client : *clients)
		{
			if (client->hash() == target)
			{
				emit messageToSend(*client->getSocket(), textMessage);
			}
		}
	}

	void ForumServer::execute(const QString &target, const RequestMessage & request)
	{
		QMutexLocker locker(&mutex);
		QString messageString = request.getMessageString();
		istringstream iss(messageString.toStdString());
		int boardID;
		int postID;
		string boardName;
		string userName;
		string password;
		switch (request.getType())
		{
		case GetBoardListRequestMessageType:
			sendBoardList(target);
			break;
		case GetPostListRequestMessageType:
			iss >> boardID;
			sendPostList(target, boardID);
			break;
		case GetCommentListRequestMessageType:
			iss >> boardID;
			iss >> postID;
			sendCommentList(target, boardID, postID);
			break;
		case RegisterRequestMessageType:
			getline(iss, userName);
			getline(iss, password);
			addNormalUser(target, QString::fromStdString(userName), QString::fromStdString(password));
			break;
		case LoginRequestMessageType:
			getline(iss, userName);
			getline(iss, password);
			login(target, QString::fromStdString(userName), QString::fromStdString(password));
			break;
		case AddBoardMessageType:
			getline(iss, boardName);
			addBoard(target, QString::fromStdString(boardName));
			break;
		case AddPostMessageType:
			iss >> boardID;
			iss.get();
			addPost(target, boardID, iss);
			break;
		case AddCommentMessageType:
			iss >> boardID;
			iss >> postID;
			iss.get();
			addComment(target, boardID, postID, iss);
			break;
		default:
			break;
		}
	}

	void ForumServer::sendBoard(const QString &target, const Board & board)
	{
		ostringstream oss;
		oss << AddBoardMessageType << STD_LINE_BREAK << board;
		emit messageToSend(target, QString::fromStdString(oss.str()));
	}

	void ForumServer::sendBoardList(const QString &target)
	{
		for (QObject *&qit : *cforum->getBoards())
		{
			sendBoard(target, *static_cast<Board*>(qit));
		}
	}

	void ForumServer::broadcastBoard(const Board & board)
	{
		for (ClientDescriptor *client : *clients)
		{
			sendBoard(client->hash(), board);
		}
	}

	void ForumServer::sendPost(const QString &target, const int boardID, const Post &post)
	{
		ostringstream oss;
		oss << AddPostMessageType << STD_LINE_BREAK << boardID << STD_LINE_BREAK << post;
		emit messageToSend(target, QString::fromStdString(oss.str()));
	}

	void ForumServer::sendPostList(const QString &target, const int boardID)
	{
		Board *board;
		board = cforum->getBoardByID(boardID);
		if (board)
		{
			for (QObject *&qit : *board->getPosts())
			{
				sendPost(target, boardID, *static_cast<Post*>(qit));
			}
		}
	}

	void ForumServer::broadcastPost(const int boardID, const Post & post)
	{
		for (ClientDescriptor *client : *clients)
		{
			sendPost(client->hash(), boardID, post);
		}
	}

	void ForumServer::sendComment(const QString &target, const int boardID, const int postID, const Comment & comment)
	{
		ostringstream oss;
		oss << AddCommentMessageType << STD_LINE_BREAK << boardID << STD_LINE_BREAK << postID << STD_LINE_BREAK << comment;
		emit messageToSend(target, QString::fromStdString(oss.str()));
	}

	void ForumServer::sendCommentList(const QString &target, const int boardID, const int postID)
	{
		Board *board;
		Post *post;
		board = cforum->getBoardByID(boardID);
		if (board)
		{
			post = board->getPostByID(postID);
			if (post)
			{
				for (QObject *&qit : *post->getComments())
				{
					sendComment(target, boardID, postID, *static_cast<Comment*>(qit));
				}
			}
		}
	}

	void ForumServer::broadcastComment(const int boardID, const int postID, const Comment & comment)
	{
		for (ClientDescriptor *client : *clients)
		{
			sendComment(client->hash(), boardID, postID, comment);
		}
	}

	void ForumServer::sendToast(const QString &target, const QString &text)
	{
		ostringstream oss;
		oss << ToastResponseMessageType << STD_LINE_BREAK << text.toStdString();
		emit messageToSend(target, QString::fromStdString(oss.str()));
	}

	void ForumServer::addNormalUser(const QString &target, const QString name, const QString password)
	{
		User *user;
		user = cforum->addNormalUser(name, password);
		if (user)
		{
			sendToast(target, REGISTER_SUCCESS_MESSAGE);
		}
	}

	void ForumServer::login(const QString &target, const QString name, const QString password)
	{
		if (name == GUEST_NAME && password == GUEST_PASSWORD)
		{
			ostringstream oss;
			oss << UpdateUserResponseMessageType << STD_LINE_BREAK << Guest();
			emit messageToSend(target, QString::fromStdString(oss.str()));
		}
		else
		{
			User *user;
			user = cforum->login(name, password);
			if (user)
			{
				ostringstream oss;
				oss << UpdateUserResponseMessageType << STD_LINE_BREAK << *user;
				emit messageToSend(target, QString::fromStdString(oss.str()));
				//sendBoardList(socket);
			}
		}
	}

	void ForumServer::addBoard(const QString &target, const QString name)
	{
		Board *board = cforum->addBoard(name);
		if (board)
		{
			sendToast(target, ADD_BOARD_SUCCESS_MESSAGE);
			broadcastBoard(*board);
		}
	}

	void ForumServer::addPost(const QString &target, const int boardID, istream & in)
	{
		Post newPost;
		in >> newPost;
		Post *realPost = cforum->addPost(boardID, newPost.getTitle(), newPost.getContent(), newPost.getAuthorID());
		if (realPost)
		{
			sendToast(target, ADD_POST_SUCCESS_MESSAGE);
			broadcastPost(boardID, *realPost);
		}
	}

	void ForumServer::addComment(const QString &target, const int boardID, const int postID, istream & in)
	{
		Comment newComment;
		in >> newComment;
		Comment *realComment = cforum->addComment(boardID, postID, newComment.getContent(), newComment.getAuthorID());
		if (realComment)
		{
			sendToast(target, ADD_COMMENT_SUCCESS_MESSAGE);
			broadcastComment(boardID, postID, *realComment);
		}
	}
}
