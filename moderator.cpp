#include "moderator.h"

namespace cforum
{
    Moderator::Moderator() : NormalUser()
    {
        type = ModeratorType;
    }

    Moderator::Moderator(const NormalUser &oldNormalUser)
        : NormalUser(oldNormalUser)
    {
    }

    Moderator::Moderator(ifstream &stream) : NormalUser(stream, ModeratorType)
    {
    }

    /**
     * @brief 将对象设置为版主，在版主的版面列表中增加版面。
     *
     * @param boardID
     */
    void Moderator::setModerator(const int boardID)
    {
        boards->insert(boardID);
    }

    /**
     * @brief 从版主的版面列表中移除版面，版面不存在则返回假。
     *
     * @param boardID
     * @return true
     * @return false
     */
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
        return static_cast<int>(boards->size());
    }

    QString Moderator::getInfo() const
    {
        return INFO_MESSAGE_MODERATOR + getBasicInfo();
    }

    /**
     * @brief 降级为普通用户，当版面列表为空时执行。
     *
     * @return NormalUser*
     */
    NormalUser * Moderator::toNormalUser() const
    {
        return new NormalUser(static_cast<const User*>(this));
    }
}
