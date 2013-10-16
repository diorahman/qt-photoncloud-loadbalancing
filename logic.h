#ifndef LOGIC_H
#define LOGIC_H

#include "LoadBalancing-cpp/inc/LoadBalancingClient.h"

class Logic : private ExitGames::LoadBalancing::Listener
{
public:
    explicit Logic();

    void run(void);
    void connect(void);

private:
    //From Common::BaseListener
    // receive and print out Photon datatypes debug out here
    virtual void debugReturn(const ExitGames::Common::JString& string);

    //From LoadBalancing::LoadBalancingListener
    // receive and print out Photon LoadBalancing debug out here
    virtual void debugReturn(ExitGames::Common::DebugLevel::DebugLevel debugLevel, const ExitGames::Common::JString& string);

    // implement your error-handling here
    virtual void connectionErrorReturn(int errorCode);
    virtual void clientErrorReturn(int errorCode);
    virtual void warningReturn(int warningCode);
    virtual void serverErrorReturn(int errorCode);

    // events, triggered by certain operations of all players in the same room
    virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player);
    virtual void leaveRoomEventAction(int playerNr);
    virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Hashtable& eventContent);

    // callbacks for operations on PhotonLoadBalancing server
    virtual void connectReturn(int errorCode, const ExitGames::Common::JString& errorString);
    virtual void disconnectReturn(void);
    virtual void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
    virtual void joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
    virtual void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
    virtual void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString);
    virtual void gotQueuedReturn(void);
    virtual void joinLobbyReturn(void);
    virtual void leaveLobbyReturn(void);

    ExitGames::LoadBalancing::Client m_client;
};

#endif // LOGIC_H
