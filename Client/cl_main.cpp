#include <iostream>
#include <string>

#include "../Network/network.h"

int main()
{
	std::string ip_addr = "127.0.0.1";
	int port = 34560;

	Client client;
	client.Connect(ip_addr, port);

	while (true)
	{
		std::string message;
		std::cin >> message;
		if (message.find("exit") != std::string::npos)
			break;

		client.Write(message);
	}
	return 0;
}