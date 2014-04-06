#include <iostream>
#include <string>

#include "../Network/network.h"

int main()
{
	std::string ip_addr = "127.0.0.1";
	int port = 34560;

	Client client;
	client.Connect(ip_addr, port);

	return 0;
}