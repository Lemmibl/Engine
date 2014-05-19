#include "NetworkServer.h"

#include "NetworkData.h"
#include "GameConsoleWindow.h"

NetworkServer::NetworkServer(GameConsoleWindow* window)
	: clientId(0),
	  iFlag(0)
{
	consoleWindow = window;
}

NetworkServer::~NetworkServer()
{
	Shutdown();
}

bool NetworkServer::Initialize(const ServerSettings& settings)
{
	// create WSADATA object
	WSADATA wsaData;

	// our sockets for the server
	listenSocket = INVALID_SOCKET;
	clientSocket = INVALID_SOCKET;

	// address info for the server to listen to
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) 
	{
		consoleWindow->PrintText("WSAStartup failed with error: " + iResult);
		return false;
	}

	// set address information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

	if ( iResult != 0 ) 
	{
		consoleWindow->PrintText("getaddrinfo failed with error: " + iResult);
		Shutdown();
		return false;
	}

	// Create a SOCKET for connecting to server
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET) 
	{
		consoleWindow->PrintText("socket failed with error: " + WSAGetLastError());
		freeaddrinfo(result);
		Shutdown();
		return false;
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;
	iResult = ioctlsocket(listenSocket, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR) 
	{
		consoleWindow->PrintText("ioctlsocket failed with error: " + WSAGetLastError());
		Shutdown();
		return false;
	}

	// Setup the TCP listening socket
	iResult = bind( listenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) 
	{
		consoleWindow->PrintText("bind failed with error: " + WSAGetLastError());
		freeaddrinfo(result);
		Shutdown();
		return false;
	}

	// no longer need address information
	freeaddrinfo(result);

	// start listening for new clients attempting to connect
	iResult = listen(listenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR) 
	{
		consoleWindow->PrintText("listen failed with error: " + WSAGetLastError());
		Shutdown();
		return false;
	}

	return true;
}

void NetworkServer::Update()
{
	// get new clients
	if(AcceptNewClient(clientId))
	{
		consoleWindow->PrintText("Client has been connected to the server. Client ID is: " + std::to_string((long long)clientId)); 
		clientId++;
	}

	PollAllClients();
}

void NetworkServer::Shutdown()
{
	if(listenSocket != INVALID_SOCKET)
	{	
		closesocket(listenSocket);
	}

	WSACleanup();
}

bool NetworkServer::AcceptNewClient(unsigned int& outId)
{
	//If we get a new client waiting, accept the connection and save the socket
	clientSocket = accept(listenSocket, NULL, NULL);

	if(clientSocket != INVALID_SOCKET) 
	{
		//disable nagle algorithm on the client's socket
		char value = 1;
		setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

		UserData newUser;
		newUser.clientSocket = clientSocket;
		newUser.textColor = CEGUI::Colour(1.0f, 1.0f, 1.0f, 1.0f);
		newUser.userName = "/";

		// insert new client into session id table
		sessions.insert(std::pair<unsigned int, UserData>(outId, newUser) );

		return true;
	}

	return false;
}

void NetworkServer::RemoveClient( unsigned int id )
{
	//Search for client in map
	auto it = sessions.find(id);

	//If there is a client
	if(it != sessions.end())
	{
		//Remove it
		int result = closesocket(it->second.clientSocket);
		if(result > 0)
		{
			consoleWindow->PrintText("Client was successfully disconnected.");
		}

		sessions.erase(it);
	}
}

void NetworkServer::PollAllClients()
{
	EventPacket packet;
	sessionsToDelete.clear();

	ZeroMemory(network_data, MAX_PACKET_SIZE);

	unsigned int dataSize = 0;
	DataPacketType type = TypeNONE;

	ZeroMemory(packet_header, DataPacketHeader::sizeOfStruct);

	for(auto iter = sessions.begin(); iter != sessions.end(); ++iter)
	{
		iFlag = 0;

		if(ReadDataHeader(iter->first, network_data, &type, &dataSize))
		{
			switch(type)
			{
				case TypeSTRING:
				{
					ReadStringData(iter->first, network_data, dataSize);
					break;
				}

				case TypeUSERDATA:
				{
					ReadUserData(iter->first, network_data, dataSize);
					break;
				}

				case TypeDISCONNECT:
				{
					sessionsToDelete.push_back(iter);
					break;
				}

				default:
				{
					break;
				}
			}
		}
	}

	//Potential cleanup
	for(int i = 0; i < sessionsToDelete.size(); ++i)
	{
		RemoveClient(sessionsToDelete[i]->first);
	}
}

int NetworkServer::ReceiveData(unsigned int client_id, char* receivingBuffer)
{
	if( sessions.find(client_id) != sessions.end() )
	{
		SOCKET currentSocket = sessions[client_id].clientSocket;

		iResult = NetworkServices::ReceiveMessage(currentSocket, receivingBuffer, MAX_PACKET_SIZE, iFlag);
		if (iResult == 0)
		{
			consoleWindow->PrintText("Connection closed.");
			closesocket(currentSocket);
		}

		return iResult;
	}
	return 0;
}

void NetworkServer::SendToAllClients(char* packets, int totalSize, int flag)
{
	//TODO: Do something with flag
	SOCKET currentSocket;
	int iSendResult;

	for(auto iter = sessions.begin(); iter != sessions.end(); ++iter)
	{
		currentSocket = iter->second.clientSocket;
		iSendResult = NetworkServices::SendMessage(currentSocket, packets, totalSize);

		if (iSendResult == SOCKET_ERROR) 
		{
			consoleWindow->PrintText("Send failed with error: " + WSAGetLastError());
			closesocket(currentSocket);
		}
	}
}

void NetworkServer::SendDummyPackets()
{
	// send action packet
	const unsigned int packet_size = sizeof(EventPacket);
	char packet_data[packet_size];

	EventPacket packet;
	packet.packet_type = ACTION_EVENT;
	packet.Serialize(packet_data);

	SendToAllClients(packet_data, packet_size);
}

void NetworkServer::ReadStringData(unsigned int client_id, char* receivingBuffer, unsigned int bufferSize)
{
	auto it = sessions.find(client_id);
	if(it != sessions.end())
	{
		NetworkServices::ReceiveMessage(it->second.clientSocket, receivingBuffer, bufferSize, iFlag);

		CEGUI::String text(receivingBuffer, bufferSize);

		consoleWindow->PrintText(it->second.userName + ": " + text, it->second.textColor);
	}
}

void NetworkServer::ReadUserData(unsigned int client_id, char* receivingBuffer, unsigned int bufferSize)
{
	auto it = sessions.find(client_id);
	if(it != sessions.end() )
	{
		//Read in the data in the form that was described by header
		NetworkServices::ReceiveMessage(it->second.clientSocket, receivingBuffer, bufferSize, iFlag);

		auto& userData = it->second;		//read in data equal to bufferSize minus the colour part
		unsigned int calculatedTextSize = (bufferSize-sizeof(CEGUI::argb_t));
		std::vector<char> tempContainer(calculatedTextSize);
		CEGUI::argb_t tempColour;

		memcpy(&tempColour, receivingBuffer, sizeof(CEGUI::argb_t));
								   //Offset by sizeof colour			  //rest of buffersize will consist of string data
		memcpy(tempContainer.data(), receivingBuffer+sizeof(CEGUI::argb_t), calculatedTextSize);

		userData.textColor = CEGUI::Colour(tempColour);
		userData.userName = CEGUI::String(tempContainer.data(), tempContainer.size());

		consoleWindow->PrintText("Server has now received new user data.");
		consoleWindow->PrintText("User name is: " + userData.userName);
	}
}

bool NetworkServer::ReadDataHeader( unsigned int client_id, char* receivingBuffer, DataPacketType* outType, unsigned int* outSize )
{
	auto it = sessions.find(client_id);
	if(it != sessions.end() )
	{
		ZeroMemory(packet_header, DataPacketHeader::sizeOfStruct);

		iResult = NetworkServices::ReceiveMessage(it->second.clientSocket, packet_header, DataPacketHeader::sizeOfStruct, iFlag);
		if(iResult == 0)
		{
			consoleWindow->PrintText("Something went wrong when trying to read data packet header.");
			return false;
		}
		else if(iResult > 0)
		{
			DataPacketHeader::Deserialize(packet_header, outType, outSize);
			return true;
		}
	}

	return false;
}
