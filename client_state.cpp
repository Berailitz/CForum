#include "client_state.h"

namespace cforum
{
    ClientState::ClientState() : QObject()
    {
    }

    ClientState::ClientState(const ClientState &oldClientState) : QObject()
    {
        initialize(&oldClientState);
    }

    ClientState::ClientState(const ClientState *oldClientState) : QObject()
    {
        initialize(oldClientState);
    }

    ClientState::~ClientState()
    {
    }

    void ClientState::initialize(const ClientState * oldClientState)
    {
        if (oldClientState != this)
        {
            userID = oldClientState->userID;
            boardID = oldClientState->boardID;
            postID = oldClientState->postID;
            commentID = oldClientState->commentID;
        }
    }

    ClientState ClientState::operator=(const ClientState & oldClientState)
    {
        initialize(&oldClientState);
        return *this;
    }
}
