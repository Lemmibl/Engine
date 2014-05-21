#include "NetworkClient.h"

#include "NetworkServices.h"
#include "GameConsoleWindow.h"
#include "NetworkData.h"

NetworkClient::NetworkClient( GameConsoleWindow* console )
	: receivingBufferLength(DEFAULT_BUFLEN),
	outFlags(0),
	iResult(0)
{
	consoleWindow = console;

	//Clear header data
	ZeroMemory(header_data, DataPacketHeader::sizeOfStruct);
	ZeroMemory(network_data, MAX_PACKET_SIZE);
}


NetworkClient::~NetworkClient()
{
	Shutdown();
}

bool NetworkClient::Connect(UserData& userData, CEGUI::String ip, CEGUI::String port)
{
	WSADATA wsaData;
	struct addrinfo *result = NULL, *ptr = NULL, hints;

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

	//Send a first initializing packet, filled with user data like user name and user text colour
	SendUserDataPacket(userData);

	return true;
}

bool NetworkClient::Update()
{
	//Read to see if we've received any data
	int packetSize = NetworkServices::ReceiveData(connectionSocket, network_data, MAX_PACKET_SIZE);

	//Extract all the data
	ReceiveDataFromServer(packetSize);

	//Then we send all data back to server...
	SendDataToServer();

	return true;
}

bool NetworkClient::ReceiveDataFromServer(int packetSize, int packetIndex)
{
	if(packetSize > 0)
	{
		//Move network data into header data array
		memcpy(header_data, network_data+packetIndex, DataPacketHeader::sizeOfStruct);

		//Extract data from header
		DataPacketType dataType;
		unsigned int dataSize;
		DataPacketHeader::Deserialize(header_data, &dataType, &dataSize);

		//Move packet index forward and subtract from packetSize
		packetIndex += DataPacketHeader::sizeOfStruct;
		packetSize -= DataPacketHeader::sizeOfStruct;

		//Act upon data
		switch(dataType)
		{
		case TypeSTRING:
			{
				ReadStringData(dataSize, packetIndex, false);
				break;
			}

		case TypeCOLOUREDSTRING:
			{
				ReadStringData(dataSize, packetIndex, true);
				break;
			}

		case TypeUSERDATA:
			{
				ReadUserData(dataSize, packetIndex);
				break;
			}

		case TypeCONNECT:
			{
				break;
			}

		case TypeDISCONNECT:
			{
				break;
			}

		default:
			{
				break;
			}
		}

		//Move index forward again and subtract size again
		packetIndex += dataSize;
		packetSize -= dataSize;


		//Call again
		ReceiveDataFromServer(packetSize, packetIndex);
	}

	return true;
}

void NetworkClient::ReadStringData(unsigned int dataSize, unsigned int dataIndex, bool extractColor)
{
	//If we are extracting color, it means that we have a 4 byte portion of the data at the end that is colour, not text.
	int textSizePortion = extractColor ? dataSize-sizeof(CEGUI::argb_t) : dataSize;

	//Create array of right size..
	std::vector<char> tempContainer(textSizePortion);

	//Copy data from the right index and of the right size into the array
	memcpy(tempContainer.data(), network_data+dataIndex, textSizePortion);

	//Create the string
	CEGUI::String tempString(tempContainer.data(), textSizePortion);

	if(extractColor)
	{
		CEGUI::argb_t argbVal = 0;
		memcpy(&argbVal, network_data+dataIndex+textSizePortion, sizeof(CEGUI::argb_t));
		CEGUI::Colour color(argbVal);

		consoleWindow->PrintText(tempString, color);
	}
	else
	{
		consoleWindow->PrintText(tempString);
	}
}

void NetworkClient::ReadDisconnectData(unsigned int dataSize, unsigned int dataIndex)
{
	//TODO... Potentially
}

void NetworkClient::ReadUserData(unsigned int dataSize, unsigned int dataIndex)
{

}

bool NetworkClient::SendDataToServer()
{
	for(auto iter = dataToSend.begin(); iter != dataToSend.end(); ++iter)
	{
		//Create and serialize data header packet
		DataPacketHeader::Serialize(header_data, iter->dataType, iter->dataVector.size());

		//Send header
		NetworkServices::SendData(connectionSocket, header_data, DataPacketHeader::sizeOfStruct);

		//Then if it's a type of packet that actually contains something, we send the body
		switch(iter->dataType)
		{
		case TypeSTRING:
			{
				NetworkServices::SendData(connectionSocket, iter->dataVector.data(), iter->dataVector.size());
				break;
			}

		case TypeUSERDATA:
			{
				NetworkServices::SendData(connectionSocket, iter->dataVector.data(), iter->dataVector.size());
				break;
			}

		}
	}

	dataToSend.clear();

	return true;
}

void NetworkClient::SendTextPacket(std::string text)
{
	size_t len = text.length();

	//We don't send if it's an empty message.....
	if(len > 0)
	{
		//Will contain the data that we're sending
		DataPacket outPacket;
		outPacket.dataType = TypeSTRING;

		//So this will both set vector size to right size and preallocate everything to nice default value (0 in the case of char)
		outPacket.dataVector.resize(len);

		//Then we move it into the vector...
		std::memcpy(outPacket.dataVector.data(), text.c_str(), len);

		//And move it into this list
		dataToSend.push_back(std::move(outPacket));
	}
}

void NetworkClient::SendUserDataPacket(UserData& userData)
{
	const unsigned int textLength = (sizeof(char) * userData.userName.length());
	const unsigned int structLength = (textLength + sizeof(CEGUI::argb_t));

	DataPacket outPacket;
	CEGUI::argb_t tempColor = userData.textColor.getARGB();

	//Initialize char array of the right size, with all values to default (in case of char, that's: 0)
	outPacket.dataVector.resize(structLength);
	outPacket.dataType = TypeUSERDATA;

	//Text in first
	memcpy(outPacket.dataVector.data(),				userData.userName.c_str(),		textLength);

	//Append color to the end. Offset with textLength
	memcpy(outPacket.dataVector.data()+textLength,	&tempColor,						sizeof(CEGUI::argb_t));

	//Insert packet
	dataToSend.push_back(std::move(outPacket));
}

void NetworkClient::Disconnect()
{
	int result = 1;

	SendDisconnectPacket();

	// shutdown the connection since no more data will be sent or read
	result = shutdown(connectionSocket, SD_SEND);

	// cleanup
	closesocket(connectionSocket);
}

void NetworkClient::SendDisconnectPacket()
{
	//Create and serialize data header packet
	DataPacketHeader::Serialize(header_data, TypeDISCONNECT, 0);

	//Send header
	NetworkServices::SendData(connectionSocket, header_data, DataPacketHeader::sizeOfStruct);
}


void NetworkClient::Shutdown()
{
	Disconnect();

	WSACleanup();
}