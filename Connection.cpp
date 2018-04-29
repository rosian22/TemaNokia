#include "Connection.h"


UID connectionUIDPool = 0;
std::mutex connectionUIDPoolLock;

UID GenerateNewUID() {
	register UID returnUID = 0;
	connectionUIDPoolLock.lock();
	returnUID = ++connectionUIDPool.id;
	connectionUIDPoolLock.unlock();
	return returnUID;
}