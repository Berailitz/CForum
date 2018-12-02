#ifndef CFORUM_MODERATOR_H
#define CFORUM_MODERATOR_H

#include "normal_user.h"

namespace cforum
{
	const QString INFO_MESSAGE_MODERATOR = QString::fromUtf8("版主 ");
	
	class Moderator : public NormalUser
	{
        Q_OBJECT
	public:
		Moderator(const NormalUser &oldNormalUser);
		Moderator(ifstream &stream);
		void setModerator(const int boardID);
		bool removeModerator(const int boardID);
		bool isModerator(const int boardID = -1) const;
		int getBoardCounter() const;
		virtual QString getInfo() const;
		NormalUser *toNormalUser() const;
	private:
		BoardSet *boards = new BoardSet();
	};
}

#endif // !CFORUM_MODERATOR_H
