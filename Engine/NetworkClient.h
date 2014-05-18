#define WIN32_LEAN_AND_MEAN

#pragma once
#include <ws2tcpip.h>
#include <ctime>
#include "CEGUI/String.h"

#include "NetworkServices.h"
#include "NetworkData.h"

class GameConsoleWindow;

class NetworkClient
{
private:
#define DEFAULT_IP "127.0.0.1"

//Port number that is unlikely to be used on client computers. Also easy to remember.
//http://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers
#define DEFAULT_PORT "22222"
#define DEFAULT_BUFLEN 512

public:
	NetworkClient(GameConsoleWindow* console);
	~NetworkClient();

	//Connect function, with default ip and port in case nothing is entered
	bool Connect(CEGUI::String ip = DEFAULT_IP, CEGUI::String port = DEFAULT_PORT);
	bool Update();
	
	int ReceivePackets(char* receivingBuffer);
	bool ReadPackets(int packetSize, char* receivedBuffer);

	void SendTextPacket(std::string text);

	//Should be temporary
	void SendDummyPacket();

	//TODO: Name this just disconnect and make sure everything is logical about the function
	void SendDisconnectPacket();

	void Disconnect();
	void Shutdown();

private:
	//Just for debugging purposes...?
	GameConsoleWindow* consoleWindow;

	//Socket that this client uses...
	SOCKET connectionSocket;
	//char receivingBuffer[DEFAULT_BUFLEN];
	char network_data[MAX_PACKET_SIZE];

	int receivingBufferLength;
	int outFlags, iResult;
};

