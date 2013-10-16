#include "NetworkLogic.h"

#if defined _EG_MARMALADE_PLATFORM
#	if defined I3D_ARCH_X86
#		if(defined _MSC_VER && !defined __clang__ && !defined __gcc__)
#			define PLAYER_NAME L"Marmalade X86 Windows"
#		else
#			define PLAYER_NAME L"Marmalade X86 OS X"
#		endif
#	elif defined I3D_ARCH_ARM
#		define PLAYER_NAME L"Marmalade ARM"
#	else
#		define PLAYER_NAME L"unknown Marmalade platform"
#	endif
#elif defined _EG_WINDOWS_PLATFORM
#	define PLAYER_NAME L"Win32"
#elif defined _EG_IPHONE_PLATFORM
#	define PLAYER_NAME L"iOS"
#elif defined _EG_IMAC_PLATFORM
#	define PLAYER_NAME L"OS X"
#elif defined _EG_ANDROID_PLATFORM
#	define PLAYER_NAME L"Android"
#elif defined _EG_BLACKBERRY_PLATFORM
#	define PLAYER_NAME L"Blackberry"
#elif defined _EG_PS3_PLATFORM
#	define PLAYER_NAME L"PS3"
#elif defined _EG_LINUX_PLATFORM
#	define PLAYER_NAME L"Linux"
#else
#	define PLAYER_NAME L"unknown platform"
#endif

ExitGames::Common::JString& NetworkLogicListener::toString(ExitGames::Common::JString& retStr, bool /*withTypes*/) const
{
	return retStr;
}

StateAccessor::StateAccessor(QObject *parent) : QObject(parent)
{
}

StateAccessor::~StateAccessor()
{
}

State StateAccessor::getState(void) const
{
	return mState;
}

void StateAccessor::setState(State newState)
{
	mState = newState;
	for(unsigned int i=0; i<mStateUpdateListeners.getSize(); i++)
		mStateUpdateListeners[i]->stateUpdate(newState);

    emit stateChanged();
}

void StateAccessor::registerForStateUpdates(NetworkLogicListener* listener)
{
	mStateUpdateListeners.addElement(listener);
}

Input NetworkLogic::getLastInput(void) const
{
	return mLastInput;
}

void NetworkLogic::setLastInput(Input newInput)
{
	mLastInput = newInput;
}

State NetworkLogic::getState(void) const
{
    return mStateAccessor->getState();
}

StateAccessor *NetworkLogic::getStateAccessor()
{
    return mStateAccessor;
}

// functions
NetworkLogic::NetworkLogic(OutputListener* listener, const wchar_t* appVersion)
#ifdef _EG_MS_COMPILER
#	pragma warning(push)
#	pragma warning(disable:4355)
#endif
	: mLoadBalancingClient(*this, L"54c7e8b7-5cee-4b58-979c-4eb969747fa4", appVersion, PLAYER_NAME)
	, mLastInput(INPUT_NON)
	, mOutputListener(listener)
#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
{
    mStateAccessor = new StateAccessor();
    mStateAccessor->setState(STATE_INITIALIZED);
	mLoadBalancingClient.setDebugOutputLevel(ExitGames::Common::DebugLevel::INFO);
	mLogger.setListener(*this);
	mLogger.setDebugOutputLevel(ExitGames::Common::DebugLevel::INFO);
}

NetworkLogic::~NetworkLogic()
{
    if(mStateAccessor) delete mStateAccessor;
}

void NetworkLogic::registerForStateUpdates(NetworkLogicListener* listener)
{
    mStateAccessor->registerForStateUpdates(listener);
}

void NetworkLogic::connect(void)
{
	mLoadBalancingClient.connect(); // specify the ip and port of your local masterserver here; call the parameterless overload instead, if you want to connect to the  exitgamescloud
    mStateAccessor->setState(STATE_CONNECTING);
}

void NetworkLogic::disconnect(void)
{
	mLoadBalancingClient.disconnect();
}

void NetworkLogic::opCreateRoom(void)
{
	// if last digits are always nearly the same, this is because of the timer calling this function being triggered every x ms with x being a factor of 10
	ExitGames::Common::JString tmp;
	mLoadBalancingClient.opCreateRoom(tmp=(int)GETTIMEMS());
    mStateAccessor->setState(STATE_JOINING);
	mOutputListener->writeLine(ExitGames::Common::JString(L"creating game \"") + tmp + L"\"");
}

void NetworkLogic::opJoinRandomRoom(void)
{
	mLoadBalancingClient.opJoinRandomRoom();
}

void NetworkLogic::run(void)
{
    if(mLastInput == INPUT_EXIT && mStateAccessor->getState() != STATE_DISCONNECTING && mStateAccessor->getState() != STATE_DISCONNECTED)
	{
		disconnect();
        mStateAccessor->setState(STATE_DISCONNECTING);
		mOutputListener->writeLine(L"terminating application");

	}
	else
	{
        State state = mStateAccessor->getState();
		switch(state)
		{
		case STATE_INITIALIZED:
			connect();
            mStateAccessor->setState(STATE_CONNECTING);
			mOutputListener->writeLine(L"connecting");
			break;
		case STATE_CONNECTING:
			break; // wait for callback
		case STATE_CONNECTED:
			switch(mLastInput)
			{
			case INPUT_CREATE_GAME: // create Game
				opCreateRoom();
				break;
			case INPUT_JOIN_RANDOM_GAME: // join Game
				opJoinRandomRoom();
                mStateAccessor->setState(STATE_JOINING);
				mOutputListener->writeLine(L"joining random game");
				break;
			default: // no or illegal input -> stay waiting for legal input
				break;
			}
			break;
		case STATE_JOINING:
			break; // wait for callback
		case STATE_JOINED:
			sendEvent();
			switch(mLastInput)
			{
			case INPUT_LEAVE_GAME: // leave Game
				mLoadBalancingClient.opLeaveRoom();
                mStateAccessor->setState(STATE_LEAVING);
				mOutputListener->writeLine(L"");
				mOutputListener->writeLine(L"leaving game room");
				break;
			default: // no or illegal input -> stay waiting for legal input
				break;
			}
			break;
		case STATE_LEAVING:
			break; // wait for callback
        case STATE_LEFT:{
            mStateAccessor->setState(STATE_CONNECTED);
        }

			break;
		case STATE_DISCONNECTING:
			break; // wait for callback
		default:
			break;
		}
	}
	mLastInput = INPUT_NON;
	mLoadBalancingClient.service();
}

void NetworkLogic::sendEvent(void)
{
#ifdef _EG_PS3_PLATFORM
	static int count = 0;
	ExitGames::Common::Hashtable data;
	data.put<int, int>(0, count++);
	mLoadBalancingClient.opRaiseEvent(false, data, 0);
	mOutputListener->write(ExitGames::Common::JString(L"s:") + count + ExitGames::Common::JString(L" "));
#else
	mLoadBalancingClient.opRaiseEvent(false, ExitGames::Common::Hashtable(), 0);
	mOutputListener->write(L"s");
#endif
}

// protocol implementations

void NetworkLogic::debugReturn(const ExitGames::Common::JString& string)
{
	mOutputListener->debugReturn(string);
}

void NetworkLogic::debugReturn(ExitGames::Common::DebugLevel::DebugLevel /*debugLevel*/, const ExitGames::Common::JString& string)
{
	debugReturn(string);
}

void NetworkLogic::connectionErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	mOutputListener->writeLine(ExitGames::Common::JString(L"connection failed with error ") + errorCode);
    mStateAccessor->setState(STATE_DISCONNECTED);
}

void NetworkLogic::clientErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	mOutputListener->writeLine(ExitGames::Common::JString(L"received error ") + errorCode + L" from client");
}

void NetworkLogic::warningReturn(int warningCode)
{
	EGLOG(ExitGames::Common::DebugLevel::WARNINGS, L"code: %d", warningCode);
	mOutputListener->writeLine(ExitGames::Common::JString(L"received warning ") + warningCode + L" from client");
}

void NetworkLogic::serverErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	mOutputListener->writeLine(ExitGames::Common::JString(L"received error ") + errorCode + " from server");
}

void NetworkLogic::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& /*playernrs*/, const ExitGames::LoadBalancing::Player& player)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
	mOutputListener->writeLine(L"");
	mOutputListener->writeLine(ExitGames::Common::JString(L"player ") + playerNr + L" " + player.getName() + L" has joined the game");
}

void NetworkLogic::leaveRoomEventAction(int playerNr)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mOutputListener->writeLine(L"");
	mOutputListener->writeLine(ExitGames::Common::JString(L"player ") + playerNr + " has left the game");
}

void NetworkLogic::customEventAction(int /*playerNr*/, nByte /*eventCode*/, const ExitGames::Common::Hashtable& eventContent)
{
	// you do not receive your own events, unless you specify yourself as one of the receivers explicitly, so you must start 2 clients, to receive the events, which you have sent, as sendEvent() uses the default receivers of opRaiseEvent() (all players in same room like the sender, except the sender itself)
	EGLOG(ExitGames::Common::DebugLevel::ALL, L"");
#ifdef _EG_PS3_PLATFORM
	int count = ExitGames::Common::ValueObject<int>(eventContent.getValue<int>(0)).getDataCopy();
	mOutputListener->write(ExitGames::Common::JString(L"r:") + count + ExitGames::Common::JString(L"\n"));
#else
	mOutputListener->write(L"r");
#endif
}

void NetworkLogic::connectReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
        mStateAccessor->setState(STATE_DISCONNECTING);
		return;
	}
	mOutputListener->writeLine(L"connected");
    mStateAccessor->setState(STATE_CONNECTED);
}

void NetworkLogic::disconnectReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mOutputListener->writeLine(L"disconnected");
    mStateAccessor->setState(STATE_DISCONNECTED);
}

void NetworkLogic::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mOutputListener->writeLine(L"opCreateRoom() failed: " + errorString);
        mStateAccessor->setState(STATE_CONNECTED);
		return;
	}
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mOutputListener->writeLine(L"game room \"" + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + "\" has been created");
	mOutputListener->writeLine(L"regularly sending dummy events now");
    mStateAccessor->setState(STATE_JOINED);
}

void NetworkLogic::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mOutputListener->writeLine(L"opJoinRoom() failed: " + errorString);
        mStateAccessor->setState(STATE_CONNECTED);
		return;
	}
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mOutputListener->writeLine(L"game room \"" + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + "\" has been successfully joined");
	mOutputListener->writeLine(L"regularly sending dummy events now");
    mStateAccessor->setState(STATE_JOINED);
}

void NetworkLogic::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mOutputListener->writeLine(L"opJoinRandomRoom() failed: " + errorString);
        mStateAccessor->setState(STATE_CONNECTED);
		return;
	}
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	mOutputListener->writeLine(L"game room \"" + mLoadBalancingClient.getCurrentlyJoinedRoom().getName() + "\" has been successfully joined");
	mOutputListener->writeLine(L"regularly sending dummy events now");
    mStateAccessor->setState(STATE_JOINED);
}

void NetworkLogic::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mOutputListener->writeLine(L"opLeaveRoom() failed: " + errorString);
        mStateAccessor->setState(STATE_DISCONNECTING);
		return;
	}
	mOutputListener->writeLine(L"game room has been successfully left");
    mStateAccessor->setState(STATE_LEFT);
}

void NetworkLogic::gotQueuedReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
}

void NetworkLogic::joinLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mOutputListener->writeLine(L"joined lobby");
}

void NetworkLogic::leaveLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	mOutputListener->writeLine(L"left lobby");
}
