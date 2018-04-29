#include "Server.h"
#include "Stream.h"

#include<iostream>

WSADATA wsaData;

BOOL Server::Init(UINT16 port)
{
	for (UINT32 i = SERVER_MAX_CONNECTIONS - 1; i > 0; i--)
	{
		availableConnections.push(i);
		connections[i].Id = i;
	}

	INT32 result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result == SOCKET_ERROR) {
		std::cout << "Failed to init wsa lib error:" << WSAGetLastError() << "\n";
		return 5;
	}

	serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSock == INVALID_SOCKET) {
		std::cout << "Failed to create server socket.\n";
		return 1;
	}

	sockaddr_in serverInfo;
	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.S_un.S_addr = ADDR_ANY;
	serverInfo.sin_port = htons(port);

	result = bind(serverSock, (const sockaddr*)&serverInfo, sizeof(sockaddr));
	if (result == SOCKET_ERROR) {
		std::cout << "Failed to bind to port " << port << std::endl;
		return 2;
	}

	result = listen(serverSock, 10);
	if (result == SOCKET_ERROR) {
		std::cout << "Failed to start listening" << std::endl;
		return 3;
	}

	listenerThread = std::thread(Listener);
	listenerThread.detach();

	return 0;
}

void Server::RemoveConnection(UID connectionId)
{
	serverLock.lock();
	availableConnections.push(connectionId);
	serverLock.unlock();
}

void Server::SendAll(MemoryStream * packet)
{
	std::lock_guard<std::mutex> lock(serverLock);

	Connection * ptr = connections + 1;

	while (ptr - connections < (SERVER_MAX_CONNECTIONS - 1)) {

		ptr->Send(packet->_raw, packet->_pos);

		ptr++;
	}
}

void Server::Listener()
{
	sockaddr_in info;
	INT32 infoSize = sizeof(info);

	while (1) {
		SOCKET newSocket = accept(serverSock, (sockaddr*)&info, &infoSize);
		if (newSocket == INVALID_SOCKET) {
			std::cout << "New connection failed \n";
			continue;
		}

		Connection * newConnection = GetNewConnection();
		if (!newConnection) {
			std::cout << "Connection refused, max connections reached!!\n";
			closesocket(newSocket);
			continue;
		}

		INT32 result = OnNewConnection(newSocket, newConnection);
		if (result) {
			std::cout << "Failed to init connection!!\n";
		}
	}
}

INT32 Server::OnNewConnection(SOCKET sock, Connection* connection)
{
	connection->sock = sock;

	//send init message here

	MemoryStream packet = MemoryStream(32);
	packet.WriteString("Hello User");

	connection->Send(packet._raw, packet._pos);
	return 0;
}

Connection * Server::GetNewConnection()
{
	UID connectionId = 0;

	serverLock.lock();
	if (availableConnections.size() > 0) {
		connectionId = availableConnections.top();
		availableConnections.pop();
	}
	serverLock.unlock();

	if (connectionId.id) {
		return connections + connectionId.id;
	}

	return nullptr;
}

Connection				Server::connections[SERVER_MAX_CONNECTIONS];
EServerStatus			Server::Status;
std::mutex				Server::serverLock;
SOCKET					Server::serverSock;
std::thread				Server::listenerThread;
std::stack<UID>			Server::availableConnections;
