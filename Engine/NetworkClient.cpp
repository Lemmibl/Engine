#include "NetworkClient.h"

#include "GameConsoleWindow.h"
#include "NetworkData.h"

NetworkClient::NetworkClient(GameConsoleWindow* console)
	:	receivingBufferLength(DEFAULT_BUFLEN),
	outFlags(1),
	iResult(1)
{
	consoleWindow = console;
}


NetworkClient::~NetworkClient()
{
	Shutdown();
}

bool NetworkClient::Connect(UserData& userData, CEGUI::String ip, CEGUI::String port)
{
	WSADATA wsaData;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	char* sendbuf = "This is a test";
	char* serverName = nullptr;

	//initial packet data
	const unsigned int packet_size = sizeof(EventPacket);
	char packet_data[packet_size];


	/*
	To execute the client, compile the complete client source code and run the executable file. 
	The client application requires that name of the computer or IP address of the computer where the server application 
	is running is passed as a command-line parameter when the client is executed. 
	If the client and server are executed on the sample computer, the client can be started as follows:
	*/

	consoleWindow->PrintText("Now trying to connect to IP: " + ip + " Port: " + port);

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult < 0) 
	{
		consoleWindow->PrintText("WSAStartup failed. Error code: " + iResult);
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family		= AF_UNSPEC;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_protocol	= IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
	if (iResult < 0) 
	{
		consoleWindow->PrintText("getaddrinfo failed. Error code: " + iResult);
		WSACleanup();
		return false;
	}

	// Attempt to connect to an address until one succeeds
	for(ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		connectionSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (connectionSocket == INVALID_SOCKET) 
		{
			consoleWindow->PrintText("Socket failed with error code: " + WSAGetLastError());
			WSACleanup();
			return false;
		}

		// Connect to server.
		iResult = connect(connectionSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) 
		{
			closesocket(connectionSocket);
			connectionSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (connectionSocket == INVALID_SOCKET) 
	{
		consoleWindow->PrintText("Unable to connect to server!");
		WSACleanup();
		return false;
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;

	//Here is an important piece of information. We are going to set our socket to be non-blocking so that 
	// it will not wait on send() and receive() functions when there is no data to send/receive. 
	// This is necessary for our multiplayer game since we'd like the game to keep going if there isn't anything to send or receive to or from a client.

	iResult = ioctlsocket(connectionSocket, FIONBIO, &iMode);
	if (iResult == SOCKET_ERROR)
	{
		consoleWindow->PrintText("ioctlsocket failed with error: " + WSAGetLastError());
		closesocket(connectionSocket);
		WSACleanup();
		exit(1);        
	}

	//Disable Nagle algorithm
	//http://forums.codeguru.com/showthread.php?136908-Winsock-and-NAGLE-algorithm
	//Windows Sockets applications can disable the Nagle algorithm for their connections by setting the TCP_NODELAY socket option.
	//char value = 1;
	//setsockopt(connectionSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

	//// cleanup
	//closesocket(connectionSocket);
	//WSACleanup();

	//Send a first initializing packet, filled with user data like user name and user text colour
	SendUserDataPacket(userData);

	return true;
}

bool NetworkClient::Update()
{
	int data_length = ReceivePackets(network_data);

	//We have received data if data_length > 0
	if (data_length > 0) 
	{
		//Read packets received. If function returns false, it means the packet was a "disconnect event" and we should break update.
		if(!ReadPackets(data_length, network_data))
		{
			return false;
		}
	}

	return true;
}

int NetworkClient::ReceivePackets(char* recvBuffer) 
{
	int iFlag = 0;
	iResult = NetworkServices::ReceiveMessage(connectionSocket, recvBuffer, MAX_PACKET_SIZE, iFlag);

	if(iResult == 0)
	{
		consoleWindow->PrintText("Connection closed.");
		closesocket(connectionSocket);
		WSACleanup();
		exit(1);
	}

	return iResult;
}

bool NetworkClient::ReadPackets(int packetSize, char* receivedBuffer )
{
	EventPacket packet;
	int i = 0;

	//While there's still more data in the packet...
	while(i < (unsigned int)packetSize) 
	{
		packet.Deserialize(&(receivedBuffer[i]));
		i += sizeof(EventPacket);

		switch (packet.packet_type) 
		{
		case ACTION_EVENT:
			{
				consoleWindow->PrintText("Received action event packet from server.");

				//Send back the same type of packet
				//SendDummyPacket();	
				break;
			}

		case DISCONNECT_EVENT:
			{
				consoleWindow->PrintText("Connection was closed.");
				Shutdown();
				return false;
			}
		default:
			{
				consoleWindow->PrintText("Error in packet types.");
				break;
			}
		}
	}

	return true;
}

void NetworkClient::SendTextPacket(std::string text)
{
	size_t len = text.length();

	//We don't send if it's an empty message.....
	if(len > 0)
	{
		//Init header.
		ZeroMemory(header_data, DataPacketHeader::sizeOfStruct);

		//Create and serialize data header packet
		DataPacketHeader::Serialize(header_data, TypeSTRING, len);

		//Send header
		NetworkServices::SendMessage(connectionSocket, header_data, DataPacketHeader::sizeOfStruct);

		//Prepare and send data packet
		const char* p = text.c_str();
		NetworkServices::SendMessage(connectionSocket, p, len);
	}
}

void NetworkClient::SendUserDataPacket( UserData& userData)
{
	unsigned int textLength = (sizeof(char) * userData.userName.length());
	const unsigned int structLength = (textLength + sizeof(CEGUI::argb_t));

	//Init header.
	ZeroMemory(header_data, DataPacketHeader::sizeOfStruct);

	//Create and serialize data header packet
	DataPacketHeader::Serialize(header_data, TypeUSERDATA, structLength);

	//Send header
	NetworkServices::SendMessage(connectionSocket, header_data, DataPacketHeader::sizeOfStruct);

	//Setup a dynamic array to hold data because I'm ultra lazy and cowardly
	std::vector<char> userDataVec(structLength);
	CEGUI::argb_t tempColor = userData.textColor.getARGB();

	//Put textColor in first ..... four? bytes. Text in rest.
	memcpy(userDataVec.data(), &tempColor, sizeof(CEGUI::argb_t));
	memcpy(userDataVec.data()+sizeof(CEGUI::argb_t), &(*userData.userName.c_str()), textLength);

	NetworkServices::SendMessage(connectionSocket, userDataVec.data(), structLength);
}

void NetworkClient::SendDummyPacket()
{
	// send action packet
	const unsigned int packet_size = sizeof(EventPacket);
	char packet_data[packet_size];

	EventPacket packet;
	packet.packet_type = ACTION_EVENT;

	packet.Serialize(packet_data);

	NetworkServices::SendMessage(connectionSocket, packet_data, packet_size);
}

void NetworkClient::SendDisconnectPacket()
{
	//Init header.
	ZeroMemory(header_data, DataPacketHeader::sizeOfStruct);

	//Create and serialize data header packet
	DataPacketHeader::Serialize(header_data, TypeDISCONNECT, 0);

	//Send header
	NetworkServices::SendMessage(connectionSocket, header_data, DataPacketHeader::sizeOfStruct);

	//Disconnect locally.
	Disconnect();
}

void NetworkClient::Disconnect()
{
	int result = 1;

	// shutdown the connection since no more data will be sent or read
	result = shutdown(connectionSocket, SD_SEND);

	// cleanup
	closesocket(connectionSocket);
}

void NetworkClient::Shutdown()
{
	Disconnect();

	WSACleanup();
}

