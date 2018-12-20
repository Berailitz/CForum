#include "forum_server.h"

namespace cforum
{
	ForumServer::ForumServer() :
		QObject(),
		cforum(new CForum()),
		server(new QWebSocketServer(QStringLiteral("WS Server"),
			QWebSocketServer::NonSecureMode,
			this)),
		clients(new QVector<ClientDescriptor *>)
    {
    }

	ForumServer::~ForumServer()
	{
		delete cforum;
		delete server;
		for (ClientDescriptor * client : *clients)
		{
			client->getSocket()->close();
			client->deleteLater();
		}
		delete clients;
	}

	bool ForumServer::listen(const int port)
	{
		if (server->listen(QHostAddress::Any, port))
		{
			emit messageReceived("Listening on " + QString::number(port) + "\n");
			connect(server, &QWebSocketServer::newConnection,
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
		QWebSocket *socket = server->nextPendingConnection();
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
		execute(*socket, message);
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

    void ForumServer::sendMessage(const QString &target, const QString &textMessage)
    {
        for (ClientDescriptor *client : *clients)
        {
            if (client->hash() == target)
            {
				sendMessage(*client->getSocket(), textMessage);
            }
        }
    }

	void ForumServer::sendMessage(QWebSocket & socket, const QString & textMessage)
	{
		socket.sendTextMessage(textMessage);
		emit messageReceived(hashSocket(socket) + " < " + textMessage + "\n");
	}

	void ForumServer::execute(QWebSocket &socket, const RequestMessage & request)
	{
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
			sendBoardList(socket);
			break;
		case GetPostListRequestMessageType:
			iss >> boardID;
			sendPostList(socket, boardID);
			break;
		case GetCommentListRequestMessageType:
			iss >> boardID;
			iss >> postID;
			sendCommentList(socket, boardID, postID);
			break;
		case RegisterRequestMessageType:
			getline(iss, userName);
			getline(iss, password);
			addNormalUser(socket, QString::fromStdString(userName), QString::fromStdString(password));
			break;
		case LoginRequestMessageType:
			getline(iss, userName);
			getline(iss, password);
			login(socket, QString::fromStdString(userName), QString::fromStdString(password));
			break;
		case AddBoardMessageType:
			getline(iss, boardName);
			addBoard(socket, QString::fromStdString(boardName));
			break;
		case AddPostMessageType:
			iss >> boardID;
			iss.get();
			addPost(socket, boardID, iss);
			break;
		case AddCommentMessageType:
			iss >> boardID;
			iss >> postID;
			iss.get();
			addComment(socket, boardID, postID, iss);
			break;
		default:
			break;
		}
	}

	void ForumServer::sendBoard(QWebSocket & socket, const Board & board)
	{
		ostringstream oss;
		oss << AddBoardMessageType << STD_LINE_BREAK << board;
		sendMessage(socket, QString::fromStdString(oss.str()));
	}

	void ForumServer::sendBoardList(QWebSocket & socket)
	{
		for (QObject *&qit : *cforum->getBoards())
		{
			sendBoard(socket, *static_cast<Board*>(qit));
		}
	}

	void ForumServer::broadcastBoard(const Board & board)
	{
		for (ClientDescriptor *client : *clients)
		{
			sendBoard(*client->getSocket(), board);
		}
	}

	void ForumServer::sendPost(QWebSocket & socket, const int boardID, const Post &post)
	{
		ostringstream oss;
		oss << AddPostMessageType << STD_LINE_BREAK << boardID << STD_LINE_BREAK << post;
		sendMessage(socket, QString::fromStdString(oss.str()));
	}

	void ForumServer::sendPostList(QWebSocket & socket, const int boardID)
	{
		Board *board;
		board = cforum->getBoardByID(boardID);
		if (board)
		{
			for (QObject *&qit : *board->getPosts())
			{
				sendPost(socket, boardID, *static_cast<Post*>(qit));
			}
		}
	}

	void ForumServer::broadcastPost(const int boardID, const Post & post)
	{
		for (ClientDescriptor *client : *clients)
		{
			sendPost(*client->getSocket(), boardID, post);
		}
	}

	void ForumServer::sendComment(QWebSocket & socket, const int boardID, const int postID, const Comment & comment)
	{
		ostringstream oss;
		oss << AddCommentMessageType << STD_LINE_BREAK << boardID << STD_LINE_BREAK << postID << STD_LINE_BREAK << comment;
		sendMessage(socket, QString::fromStdString(oss.str()));
	}

	void ForumServer::sendCommentList(QWebSocket & socket, const int boardID, const int postID)
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
					sendComment(socket, boardID, postID, *static_cast<Comment*>(qit));
				}
			}
		}
	}

	void ForumServer::broadcastComment(const int boardID, const int postID, const Comment & comment)
	{
		for (ClientDescriptor *client : *clients)
		{
			sendComment(*client->getSocket(), boardID, postID, comment);
		}
	}

	void ForumServer::sendToast(QWebSocket & socket, const QString &text)
	{
		ostringstream oss;
		oss << ToastResponseMessageType << STD_LINE_BREAK << text.toStdString();
		sendMessage(socket, QString::fromStdString(oss.str()));
	}

	void ForumServer::addNormalUser(QWebSocket & socket, const QString name, const QString password)
	{
		User *user;
		user = cforum->addNormalUser(name, password);
		if (user)
		{
			sendToast(socket, REGISTER_SUCCESS_MESSAGE);
		}
	}

	void ForumServer::login(QWebSocket & socket, const QString name, const QString password)
	{
		User *user;
		user = cforum->login(name, password);
		if (user)
		{
			ostringstream oss;
			oss << UpdateUserResponseMessageType << STD_LINE_BREAK << *user;
			sendMessage(socket, QString::fromStdString(oss.str()));
			//sendBoardList(socket);
		}
	}

	void ForumServer::addBoard(QWebSocket & socket, const QString name)
	{
		Board *board = cforum->addBoard(name);
		if (board)
		{
			sendToast(socket, ADD_BOARD_SUCCESS_MESSAGE);
			broadcastBoard(*board);
		}
	}

	void ForumServer::addPost(QWebSocket & socket, const int boardID, istream & in)
	{
		Post newPost;
		in >> newPost;
		Post *realPost = cforum->addPost(boardID, newPost.getTitle(), newPost.getContent(), newPost.getAuthorID());
		if (realPost)
		{
			sendToast(socket, ADD_POST_SUCCESS_MESSAGE);
			broadcastPost(boardID, *realPost);
		}
	}

	void ForumServer::addComment(QWebSocket & socket, const int boardID, const int postID, istream & in)
	{
		Comment newComment;
		in >> newComment;
		Comment *realComment = cforum->addComment(boardID, postID, newComment.getContent(), newComment.getAuthorID());
		if (realComment)
		{
			sendToast(socket, ADD_COMMENT_SUCCESS_MESSAGE);
			broadcastComment(boardID, postID, *realComment);
		}
	}
}
