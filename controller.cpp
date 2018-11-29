#include "controller.h"

namespace cforum
{
    Controller::Controller(QQmlApplicationEngine *engine) : 
		QObject(), engine(engine),
		cforum(new CForum()),
		defaultUser(new Guest()),
		defaultBoard(new Board(-1, "DefaultBoard")),
		defaultThread(new Thread(-1, "DefaultThread", -1, "DefaultContent"))
	{
	}

    Controller::~Controller()
	{
	}

	Thread *Controller::getThread(const int boardID, const int threadID) const
	{
		Board *board = cforum->getBoardByID(boardID);
		if (board)
		{
			Thread *thread = board->getThreadByID(threadID);
			if (thread)
			{
				return thread;
			}
			else
			{
				return defaultThread;
			}
		}
		else
		{
			return defaultThread;
		}
	}

	User *Controller::registerUser(const QString newUserName, const QString newPassword)
    {
		User *user = new NormalUser(cforum->users->size() + 1, newUserName, newPassword);
        cforum->users->push_back(user);
		qDebug() << REGISTER_SUCCESS_MESSAGE << newUserName << ":" << newPassword;
		emit messageSent(REGISTER_SUCCESS_MESSAGE);
        login(newUserName, newPassword);
		return user;
	}

	QObject *Controller::login(const QString userName, const QString password)
	{
        User *newUser = getUserByName(userName);
        if (newUser && newUser->isPasswordCorrect(password))
		{
            qDebug() << LOGIN_SUCCESS_MESSAGE << userName;
            emit messageSent(LOGIN_SUCCESS_MESSAGE);
			emit forumOpened();
			return newUser;
		}
        else
        {
            qDebug() << LOGIN_FAILED_MESSAGE << userName;
            emit messageSent(LOGIN_FAILED_MESSAGE);
			return defaultUser;
        }
	}

    void Controller::setModerator(const int boardID, const QString userName)
	{
		Board *board = cforum->getBoardByID(boardID);
		if (board)
		{
			if (userName.length() > 0)
			{
				const User *user = getUserByName(userName);
				board->setModerator(user->id);
			}
			else
			{
				board->removeModerator();
			}
		}
		else
		{
			emit messageSent(NO_BOARD_MESSAGE);
		}
	}

    QString Controller::getUsername(const int userID) const
    {
        const User *user = cforum->getUserByID(userID);
        if (user != nullptr)
        {
            return user->userName;
        }
        else
        {
            return "";
        }
    }

    void Controller::addBoard(const QString boardName)
	{
        cforum->boards->push_back(new Board(cforum->boards->size() + 1, boardName));
	}

    void Controller::postThread(const int userID, const int boardID, const QString title, const QString content)
	{
		Board *board = cforum->getBoardByID(boardID);
		if (board)
		{
			Thread *thread = new Thread(board->threads->size() + 1, content, userID, title);
			board->post(thread);
			qDebug() << POST_SUCCESS_MESSAGE << thread->title;
			emit messageSent(POST_SUCCESS_MESSAGE);
		}
		else
		{
			emit messageSent(NO_BOARD_MESSAGE);
		}
	}

    void Controller::deleteThread(const int userID, const int boardID, const int threadID)
	{
		User *user = cforum->getUserByID(userID);
		Board *board = cforum->getBoardByID(boardID);
		if (user)
		{
			if (board)
			{
				Thread *thread = board->getThreadByID(threadID);
				if (thread && canDeleteThread(user, board, thread))
				{
					QQmlContext *ctxt = engine->rootContext();
					ctxt->setContextProperty("threadListModel", QVariant::fromValue(*defaultBoard->threads));
					ctxt->setContextProperty("commentListModel", QVariant::fromValue(defaultThread->comments));
					board->remove(threadID);
					qDebug() << DELETE_SUCCESS_MESSAGE << threadID;
					emit messageSent(DELETE_SUCCESS_MESSAGE);
				}
				else
				{
					qDebug() << DELETE_FAILED_MESSAGE << threadID;
					emit messageSent(DELETE_FAILED_MESSAGE);
				}
			}
			else
			{
				emit messageSent(NO_BOARD_MESSAGE);
			}
		}
		else
		{
			emit messageSent(NO_USER_MESSAGE);
		}
	}

    void Controller::postComment(const int userID, const int boardID, const int threadID, const QString content)
	{
		User *user = cforum->getUserByID(userID);
		Board *board = cforum->getBoardByID(boardID);
		if (user)
		{
			if (board)
			{
				Thread *thread = board->getThreadByID(threadID);
				if (thread)
				{
					Comment *newComment = new Comment(thread->comments.size() + 1, content, user->id);
					thread->post(newComment);
					qDebug() << POST_SUCCESS_MESSAGE << newComment->content;
					emit messageSent(POST_SUCCESS_MESSAGE);
				}
				else
				{
					qDebug() << DELETE_FAILED_MESSAGE << threadID;
					emit messageSent(DELETE_FAILED_MESSAGE);
				}
			}
			else
			{
				emit messageSent(NO_BOARD_MESSAGE);
			}
		}
		else
		{
			emit messageSent(NO_USER_MESSAGE);
		}
	}

    void Controller::deleteComment(const int userID, const int boardID, const int threadID, const int commentID)
	{
		User *user = cforum->getUserByID(userID);
		Board *board = cforum->getBoardByID(boardID);
		if (user)
		{
			if (board)
			{
				Thread *thread = board->getThreadByID(threadID);
				if (thread)
				{
					Comment *comment = thread->getCommentByID(commentID);
					if (comment && canDeleteComment(user, board, thread, comment))
					{
						thread->remove(commentID);
						qDebug() << DELETE_SUCCESS_MESSAGE << commentID;
						emit messageSent(DELETE_SUCCESS_MESSAGE);
					}
					else
					{
						qDebug() << DELETE_FAILED_MESSAGE << commentID;
						emit messageSent(DELETE_FAILED_MESSAGE);
					}
				}
				else
				{
					qDebug() << DELETE_FAILED_MESSAGE << threadID;
					emit messageSent(DELETE_FAILED_MESSAGE);
				}
			}
			else
			{
				emit messageSent(NO_BOARD_MESSAGE);
			}
		}
		else
		{
			emit messageSent(NO_USER_MESSAGE);
		}
	}

    void Controller::load(const QString path)
	{
        cforum->load(path.toStdString());
	}

    void Controller::save(const QString path) const
	{
        cforum->save(path.toStdString());
	}

	bool Controller::canDeleteThread(const User * user, const Board * board, const Thread * thread)
	{
		return thread->canDelete() && (user->isAdmin() || board->isModerator(user->id) || user->id == thread->authorID);
	}

	bool Controller::canDeleteComment(const User * user, const Board * board, const Thread * thread, const Comment * comment)
	{
		return user->isAdmin() || board->isModerator(user->id) || user->id == comment->authorID;
	}

	User * Controller::getUserByName(const QString userName)
	{
		for (User *user : *cforum->users)
		{
			if (user->userName == userName)
			{
				return user;
			}
		}
		return nullptr;
	}
}
