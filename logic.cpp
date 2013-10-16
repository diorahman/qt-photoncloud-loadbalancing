#include "logic.h"

Logic::Logic() : m_client(*this, L"54c7e8b7-5cee-4b58-979c-4eb969747fa4", L"1.0", "diorahman")
{
}

void Logic::run()
{

}

void Logic::connect()
{
    m_client.connect();
}

void Logic::debugReturn(const ExitGames::Common::JString &string)
{

}

void Logic::debugReturn(ExitGames::Common::DebugLevel::DebugLevel debugLevel, const ExitGames::Common::JString &string)
{

}

void Logic::connectionErrorReturn(int errorCode)
{

}

void Logic::clientErrorReturn(int errorCode)
{

}

void Logic::warningReturn(int warningCode)
{

}

void Logic::serverErrorReturn(int errorCode)
{

}

void Logic::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int> &playernrs, const ExitGames::LoadBalancing::Player &player)
{

}

void Logic::leaveRoomEventAction(int playerNr)
{

}

void Logic::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Hashtable &eventContent)
{

}

void Logic::connectReturn(int errorCode, const ExitGames::Common::JString &errorString)
{

}

void Logic::disconnectReturn()
{

}

void Logic::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable &gameProperties, const ExitGames::Common::Hashtable &playerProperties, int errorCode, const ExitGames::Common::JString &errorString)
{

}

void Logic::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable &gameProperties, const ExitGames::Common::Hashtable &playerProperties, int errorCode, const ExitGames::Common::JString &errorString)
{

}

void Logic::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable &gameProperties, const ExitGames::Common::Hashtable &playerProperties, int errorCode, const ExitGames::Common::JString &errorString)
{

}

void Logic::leaveRoomReturn(int errorCode, const ExitGames::Common::JString &errorString)
{

}

void Logic::gotQueuedReturn()
{

}

void Logic::joinLobbyReturn()
{

}

void Logic::leaveLobbyReturn()
{

}
