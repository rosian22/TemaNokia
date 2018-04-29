#include<iostream>
#include <string>

#include "Server.h"
#include "Stream.h"

int main() {

	INT32 result = Server::Init(10001);
	if (result) {
		std::cout << "Server init failed!!\n";
		std::cin.get();
		return result;
	}

	std::string msg;
	while (1) {
		std::getline(std::cin, msg);

		if (msg == "./exit") {
			break;
		}
		else if (msg == "./sendall") {
			std::string message;
			std::cout << "Messsage:";
			std::cin >> message;

			MemoryStream packet = MemoryStream(message.size() + 1);
			packet.WriteString(message.c_str());
			Server::SendAll(&packet);
		}

		Sleep(100);
	}

	return 0;
}