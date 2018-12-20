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
		default:
			break;
		}
	}

	void ForumServer::sendBoardList(QWebSocket & socket)
	{
		for (QObject *&qit : *cforum->getBoards())
		{
			ostringstream oss;
			Board *bit = static_cast<Board*>(qit);
			oss << AddBoardMessageType << STD_LINE_BREAK << *bit;
			sendMessage(socket, QString::fromStdString(oss.str()));
		}
	}

	void ForumServer::sendPostList(QWebSocket & socket, const int boardID)
	{
		Board *board;
		board = cforum->getBoardByID(boardID);
		if (board)
		{
			for (QObject *&qit : *board->getPosts())
			{
				ostringstream oss;
				Post *pit = static_cast<Post*>(qit);
				oss << AddPostMessageType << STD_LINE_BREAK << boardID << STD_LINE_BREAK << *pit;
				sendMessage(socket, QString::fromStdString(oss.str()));
			}
		}
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
					ostringstream oss;
					Comment *cit = static_cast<Comment*>(qit);
					oss << AddCommentMessageType << STD_LINE_BREAK << boardID << STD_LINE_BREAK << postID << STD_LINE_BREAK << *cit;
					sendMessage(socket, QString::fromStdString(oss.str()));
				}
			}
		}
	}

	void ForumServer::addNormalUser(QWebSocket & socket, const QString name, const QString password)
	{
		User *user;
		user = cforum->addNormalUser(name, password);
		if (user)
		{
			ostringstream oss;
			oss << ToastResponseMessageType << STD_LINE_BREAK << REGISTER_SUCCESS_MESSAGE;
			sendMessage(socket, QString::fromStdString(oss.str()));
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
			ostringstream oss;
			oss << AddBoardMessageType << STD_LINE_BREAK << *board;
			sendMessage(socket, QString::fromStdString(oss.str()));
		}
	}
}
