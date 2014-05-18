#include "NetworkServer.h"

#include "NetworkData.h"
#include "GameConsoleWindow.h"

NetworkServer::NetworkServer(GameConsoleWindow* window)
	: clientId(0)
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
		consoleWindow->PrintText("Client has been connected to the server. Client ID is: " + static_cast<unsigned char>(clientId)); 
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

		// insert new client into session id table
		sessions.insert(std::pair<unsigned int, SOCKET>(outId, clientSocket) );

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
		sessions.erase(it);
	}
}

void NetworkServer::PollAllClients()
{
	EventPacket packet;
	sessionsToDelete.clear();

	ZeroMemory(network_data, MAX_PACKET_SIZE);

	for(auto iter = sessions.begin(); iter != sessions.end(); ++iter)
	{
		ReadStringData(iter->first, network_data);

		//int i = 0;

		////While packet still contains information
		//while (i < (unsigned int)data_length) 
		//{
		//	//We deserialize data
		//	packet.Deserialize(&(network_data[i]));
		//	i += sizeof(EventPacket);

		//	switch (packet.packet_type) 
		//	{
		//	case INIT_CONNECTION:
		//		{
		//			consoleWindow->PrintText("Server received init packet from client.");
		//			SendDummyPackets();
		//			break;
		//		}
		//	case ACTION_EVENT:
		//		{
		//			consoleWindow->PrintText("Server received action event packet from client.");
		//			SendDummyPackets();
		//			break;
		//		}
		//	case DISCONNECT_EVENT:
		//		{
		//			consoleWindow->PrintText("Server received disconnect event packet from client. Removing client.");	
		//			sessionsToDelete.push_back(iter);
		//			break;
		//		}
		//	default:
		//		{
		//			consoleWindow->PrintText("Error in packet types.");
		//			break;
		//		}
		//	}
		//}
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
		int inFlag = 0;
		SOCKET currentSocket = sessions[client_id];

		iResult = NetworkServices::ReceiveMessage(currentSocket, receivingBuffer, MAX_PACKET_SIZE, inFlag);
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
		currentSocket = iter->second;
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

void NetworkServer::ReadStringData(unsigned int client_id, char* receivingBuffer)
{
	if(sessions.find(client_id) != sessions.end() )
	{
		int inFlag = 0;
		SOCKET currentSocket = sessions[client_id];

		// recv
		int flag = 0;
		int len = MAX_PACKET_SIZE;
		char* p = receivingBuffer;
		int n = NetworkServices::ReceiveMessage(currentSocket, p, len, flag);

		if(n > 0)
		{
			while (len > 0 && (n = NetworkServices::ReceiveMessage(currentSocket, p, len, flag)) > 0 ) 
			{
				p += n;
				len -= n;
			}

			if ( len > 0 || n < 0 ) 
			{
				// oops, something went wrong
			}

			std::string text(p);

			consoleWindow->PrintText("Server received text: " + text);
		}
	}
}
