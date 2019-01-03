#include "forum_server.h"

namespace cforum
{
	ForumServer::ForumServer() :
		QWebSocketServer(QStringLiteral("WS Server"),
			QWebSocketServer::NonSecureMode),
		cforum(new CForum()),
		clients(new QVector<ClientDescriptor *>)
    {
		connect(this, qOverload<const QString &, const QString &>(&ForumServer::messageToSend),
			this, qOverload<const QString &, const QString &>(&ForumServer::onMessageToSend));
		connect(this, &QWebSocketServer::newConnection,
			this, &ForumServer::onNewConnection);
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

	bool ForumServer::start(const QString portString)
	{
		int port = portString.toInt();
		return start(port);
	}

	bool ForumServer::start(const int port)
	{
		if (isListening())
		{
			emit messageReceived(SERVER_ALREADY_START_MESSAGE);
			return true;
		}
		else
		{
			if (QWebSocketServer::listen(QHostAddress::Any, port))
			{
				emit messageReceived(SERVER_START_MESSAGE + serverUrl().toDisplayString() + "\n");
				return true;
			}
			else
			{
				emit messageReceived(SERVER_START_ERROR_MESSAGE);
				return false;
			}
		}
	}

	void ForumServer::stop()
	{
		close();
		emit messageReceived(SERVER_STOP_MESSAGE);
	}

	bool ForumServer::load(const fs::path path)
	{
		lastPath = path;
		return cforum->load(path);
	}

	bool ForumServer::save() const
	{
		return save(lastPath);
	}

	bool ForumServer::save(const fs::path path) const
	{
		return cforum->save(path);
	}

	void ForumServer::onErrorRaised(const QString message)
	{
		emit messageReceived(message + "\n");
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

	void ForumServer::sendMessage(QWebSocket & socket, const QString & textMessage)
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
				sendMessage(*client->getSocket(), textMessage);
			}
		}
	}

	void ForumServer::execute(const QString &target, const RequestMessage & request)
	{
		QString messageString = request.getMessageString();
		istringstream iss(messageString.toStdString());
		int boardID, postID, commentID, userID;
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
		case RemovePostMessageType:
			iss >> boardID;
			iss >> postID;
			iss >> userID;
			removePost(target, boardID, postID, userID);
			break;
		case AddCommentMessageType:
			iss >> boardID;
			iss >> postID;
			iss.get();
			addComment(target, boardID, postID, iss);
			break;
		case RemoveCommentMessageType:
			iss >> boardID;
			iss >> postID;
			iss >> commentID;
			iss >> userID;
			removeComment(target, boardID, postID, commentID, userID);
			break;
		default:
			break;
		}

		QMutexLocker userLocker(&userMutex);
		QMutexLocker contentLocker(&contentMutex);
		QMutexLocker fileLocker(&fileMutex);
		save();
	}

	void ForumServer::sendBoard(const QString &target, const Board & board)
	{
		ostringstream oss;
		oss << AddBoardMessageType << STD_LINE_BREAK << board;
		emit messageToSend(target, QString::fromStdString(oss.str()));
	}

	void ForumServer::sendBoardList(const QString &target)
	{
		QMutexLocker contentLocker(&contentMutex);
		for (QObject *&qit : *cforum->getBoards())
		{
			sendBoard(target, *static_cast<Board*>(qit));
		}
	}

	void ForumServer::broadcastBoard(const Board & board)
	{
		ClientHashList *hashList = getAllClientHash();
		for (QString hash : *hashList)
		{
			sendBoard(hash, board);
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
		QMutexLocker contentLocker(&contentMutex);
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
		ClientHashList *hashList = getAllClientHash();
		for (QString hash : *hashList)
		{
			sendPost(hash, boardID, post);
		}
	}

	void ForumServer::broadcastPostDeletion(const int boardID, const int postID)
	{
		ClientHashList *hashList = getAllClientHash();
		RequestMessage message;
		message.removePost(boardID, postID, -1);
		for (QString hash : *hashList)
		{
			emit messageToSend(hash, message.dump());
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
		QMutexLocker contentLocker(&contentMutex);
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
		ClientHashList *hashList = getAllClientHash();
		for (QString hash : *hashList)
		{
			sendComment(hash, boardID, postID, comment);
		}
	}

	void ForumServer::broadcastCommentDeletion(const int boardID, const int postID, const int commentID)
	{
		ClientHashList *hashList = getAllClientHash();
		RequestMessage message;
		message.removeComment(boardID, postID, commentID, -1);
		for (QString hash : *hashList)
		{
			emit messageToSend(hash, message.dump());
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
		QMutexLocker userLocker(&userMutex);
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
			QMutexLocker userLocker(&userMutex);
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
		QMutexLocker contentLocker(&contentMutex);

		Board *board = cforum->addBoard(name);
		if (board)
		{
			sendToast(target, ADD_BOARD_SUCCESS_MESSAGE);
			broadcastBoard(*board);
		}
	}

	void ForumServer::addPost(const QString &target, const int boardID, istream & in)
	{
		QMutexLocker userLocker(&userMutex);
		QMutexLocker contentLocker(&contentMutex);

		Post newPost;
		in >> newPost;
		Post *realPost = cforum->addPost(boardID, newPost.getTitle(), newPost.getContent(), newPost.getAuthorID());
		userLocker.unlock();

		if (realPost)
		{
			sendToast(target, ADD_POST_SUCCESS_MESSAGE);
			broadcastPost(boardID, *realPost);
		}
	}

	void ForumServer::removePost(const QString &target, const int boardID, const int postID, const int userID)
	{
		QMutexLocker userLocker(&userMutex);
		QMutexLocker contentLocker(&contentMutex);

		bool result = cforum->removePost(boardID, postID, userID);
		if (result)
		{
			sendToast(target, REMOVE_POST_SUCCESS_MESSAGE);
			broadcastPostDeletion(boardID, postID);
		}
		else
		{
			sendToast(target, REMOVE_POST_ERROR_MESSAGE);
		}
	}

	void ForumServer::addComment(const QString &target, const int boardID, const int postID, istream & in)
	{
		QMutexLocker userLocker(&userMutex);
		QMutexLocker contentLocker(&contentMutex);

		Comment newComment;
		in >> newComment;
		Comment *realComment = cforum->addComment(boardID, postID, newComment.getContent(), newComment.getAuthorID());
		userLocker.unlock();

		if (realComment)
		{
			sendToast(target, ADD_COMMENT_SUCCESS_MESSAGE);
			broadcastComment(boardID, postID, *realComment);
		}
	}

	void ForumServer::removeComment(const QString & target, const int boardID, const int postID, const int commentID, const int userID)
	{
		QMutexLocker userLocker(&userMutex);
		QMutexLocker contentLocker(&contentMutex);

		bool result = cforum->removeComment(boardID, postID, commentID, userID);
		if (result)
		{
			sendToast(target, REMOVE_COMMENT_SUCCESS_MESSAGE);
			broadcastCommentDeletion(boardID, postID, commentID);
		}
		else
		{
			sendToast(target, REMOVE_COMMENT_ERROR_MESSAGE);
		}
	}

	ClientHashList * ForumServer::getAllClientHash()
	{
		ClientHashList *hashList = new ClientHashList();
		QMutexLocker clientsLocker(&clientsMutex);
		for (ClientDescriptor *client : *clients)
		{
			hashList->push_back(client->hash());
		}
		return hashList;
	}
}
