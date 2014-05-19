#define WIN32_LEAN_AND_MEAN
#pragma once
#include "NetworkServices.h"
#include "NetworkData.h"
#include <ws2tcpip.h>
#include <unordered_map>
#include <CEGUI/CEGUI.h>

class GameConsoleWindow;

class NetworkServer
{
private:
//Port number that is unlikely to be used on client computers. Also easy to remember.
//http://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers
#define DEFAULT_PORT "22222"
#define DEFAULT_BUFLEN 512

public:
	struct ServerSettings
	{
	};

	NetworkServer(GameConsoleWindow* window);
	~NetworkServer();

	//Potentially receive a port? A password?
	bool Initialize(const ServerSettings& settings);

	bool AcceptNewClient(unsigned int& outId);
	void RemoveClient(unsigned int id);

	int ReceiveData(unsigned int client_id, char* receivingBuffer);

	//See if any client has sent any data since last time
	void PollAllClients();

	//Returns true on success
	bool ReadDataHeader(unsigned int client_id, char* receivingBuffer, DataPacketType* outType, unsigned int* outSize);

	void ReadStringData(unsigned int client_id, char* receivingBuffer, unsigned int bufferSize);
	void ReadUserData(unsigned int client_id, char* receivingBuffer, unsigned int bufferSize);

	// send data to all clients
	void SendToAllClients(char * packets, int totalSize, int flag = 0);

	//Just temporary to test this stuff out
	void SendDummyPackets();

	void Update();
	void Shutdown();

private:
	GameConsoleWindow* consoleWindow;

	// data buffer
	char network_data[MAX_PACKET_SIZE];
	char packet_header[DataPacketHeader::sizeOfStruct];

	// Socket to listen for new connections
	SOCKET listenSocket;

	// Socket to give to the clients
	SOCKET clientSocket;

	// for error checking return values
	int iResult, iFlag;
	unsigned int clientId;

	// table to keep track of each client's socket
	std::unordered_map<unsigned int, UserData> sessions;
	std::vector<std::unordered_map<unsigned int, UserData>::iterator> sessionsToDelete;
};

