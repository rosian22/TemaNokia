#pragma once

#include "typedefs.h"
#include "Connection.h"

#include <mutex>
#include <thread>
#include <stack>

struct MemoryStream;

struct Server
{
	static	Connection connections[SERVER_MAX_CONNECTIONS];
	static	EServerStatus Status;
	static	std::mutex serverLock;
	static	SOCKET serverSock;
	static	std::thread listenerThread;
	static	std::stack<UID> availableConnections;


	static BOOL Init(UINT16 port);

	static void RemoveConnection(UID connectionId);
	static void SendAll(MemoryStream* packet);
private:
	static void Listener();
	static INT32 OnNewConnection(SOCKET sock, Connection* connection);
	static Connection* GetNewConnection();
};

