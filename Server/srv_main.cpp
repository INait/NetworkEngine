#include <iostream>
#include <string>

#include "../Network/network.h"

int main()
{
	Server server;

	std::string ip_addr = "0.0.0.0";
	int port = 34560;

	server.Listen(ip_addr, port);

	return 0;
}