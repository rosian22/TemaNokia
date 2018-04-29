#pragma once

#include "typeDefs.h"

#include<mutex>

#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

struct Connection
{
	UID Id;
	SOCKET sock;
	UINT32 messageCount;

	std::mutex cLock;

	const BOOL IsValid() {
		register BOOL isValid;

		cLock.lock();
		isValid = sock == 0 ? FALSE : TRUE;
		cLock.unlock();

		return isValid;
	}

	INT32 Send(const UINT8 * buffer, UINT32 size) {
		return send(sock, (const char*)buffer, size, 0);
	}
	INT32 Receive(UINT8 * buffer, UINT32 size) {
		return recv(sock, (char*)buffer, size, 0);
	}
};