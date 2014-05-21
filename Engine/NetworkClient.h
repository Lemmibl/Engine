#define WIN32_LEAN_AND_MEAN

#pragma once
#include <ws2tcpip.h>
#include <ctime>
#include <list>

#include "CEGUI/String.h"
#include "NetworkData.h"

class NetworkServer;
class GameConsoleWindow;

class NetworkClient
{
private:
//Port number that is unlikely to be used on client computers. Also easy to remember.
//http://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers
#define DEFAULT_PORT "22222"
#define DEFAULT_BUFLEN 512
#define DEFAULT_IP "127.0.0.1"

public:
	NetworkClient(GameConsoleWindow* console);
	~NetworkClient();

	//Connect function, with default ip and port in case nothing is entered
	bool Connect(UserData& userData, CEGUI::String ip = DEFAULT_IP, CEGUI::String port = DEFAULT_PORT);
	bool Update();

	void Disconnect();
	void Shutdown();

	void SendTextPacket(std::string text);
	void SendUserDataPacket(UserData& userData);

private:
	//Recursive function to get all data from server
	bool ReceiveDataFromServer(int packetSize = 0, int packetIndex = 0);

	//Size defines how much data we'll read and index defines where to start reading
	void ReadStringData(unsigned int dataSize, unsigned int dataIndex, bool extractColor);

	//Size defines how much data we'll read and index defines where to start reading
	void ReadDisconnectData(unsigned int dataSize, unsigned int dataIndex);

	//Size defines how much data we'll read and index defines where to start reading
	void ReadUserData(unsigned int dataSize, unsigned int dataIndex);

	//Send functions
	bool SendDataToServer();

	//TODO: Name this just disconnect and make sure everything is logical about the function
	void SendDisconnectPacket();

private:
	//Just for debugging purposes...?
	GameConsoleWindow* consoleWindow;

	//Socket that this client uses...
	SOCKET connectionSocket;

	char network_data[MAX_PACKET_SIZE];
	char header_data[DataPacketHeader::sizeOfStruct];

	//Container for things to send when Update comes around
	std::list<DataPacket> dataToSend;

	int receivingBufferLength;
	int outFlags, iResult;
};

