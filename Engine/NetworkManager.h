#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <ctime>
#include <string>

class GameConsoleWindow;

class NetworkManager
{
private:
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

public:
	NetworkManager();
	~NetworkManager();

	bool Connect(GameConsoleWindow* console, std::string ip = DEFAULT_IP, std::string port = DEFAULT_PORT);
};

