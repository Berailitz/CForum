#include "moderator.h"

namespace cforum
{
	Moderator::Moderator(const NormalUser &oldNormalUser) : NormalUser(oldNormalUser)
	{
	}

	Moderator::Moderator(ifstream &stream) : NormalUser(stream, ModeratorType)
	{
	}

	void Moderator::setModerator(const int boardID)
	{
		boards->insert(boardID);
	}

	bool Moderator::removeModerator(const int boardID)
	{
		BoardSet::iterator bit = boards->find(boardID);
		if (bit != boards->end())
		{
			boards->erase(bit);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Moderator::isModerator(const int boardID) const
	{
		if (boardID == -1 || boards->find(boardID) != boards->end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	int Moderator::getBoardCounter() const
	{
		return boards->size();
	}

	QString Moderator::getInfo() const
	{
		return INFO_MESSAGE_MODERATOR + getBasicInfo();
	}

	NormalUser * Moderator::toNormalUser() const
	{
		return new NormalUser(static_cast<const User*>(this));
	}
}
