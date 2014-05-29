#include "NetworkServer.h"

#include "NetworkServices.h"
#include "NetworkData.h"
#include "GameConsoleWindow.h"

NetworkServer::NetworkServer(GameConsoleWindow* window)
	: clientId(0),
	iFlag(0),
	serverColour(1.0f, 0.0f, 1.0f, 1.0f)
{
	consoleWindow = window;

	//Init and zero all containers.
	ZeroMemory(network_data, MAX_PACKET_SIZE);
	ZeroMemory(packet_header, DataPacketHeader::sizeOfStruct);

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
		consoleWindow->PrintText("WSAStartup failed with error: " + iResult, serverColour);
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
		consoleWindow->PrintText("getaddrinfo failed with error: " + iResult, serverColour);
		Shutdown();
		return false;
	}

	// Create a SOCKET for connecting to server
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET) 
	{
		consoleWindow->PrintText("socket failed with error: " + WSAGetLastError(), serverColour);
		freeaddrinfo(result);
		Shutdown();
		return false;
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;
	iResult = ioctlsocket(listenSocket, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR) 
	{
		consoleWindow->PrintText("ioctlsocket failed with error: " + WSAGetLastError(), serverColour);
		Shutdown();
		return false;
	}

	// Setup the TCP listening socket
	iResult = bind( listenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) 
	{
		consoleWindow->PrintText("bind failed with error: " + WSAGetLastError(), serverColour);
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
		consoleWindow->PrintText("listen failed with error: " + WSAGetLastError(), serverColour);
		Shutdown();
		return false;
	}

	return true;
}

void NetworkServer::Shutdown()
{
	if(listenSocket != INVALID_SOCKET)
	{	
		closesocket(listenSocket);
	}

	WSACleanup();
}

bool NetworkServer::AddClient(UserID& outId)
{
	//If we get a new client waiting, accept the connection and save the socket
	clientSocket = accept(listenSocket, NULL, NULL);

	if(clientSocket != INVALID_SOCKET) 
	{
		//disable nagle algorithm on the client's socket
		char val = 1;
		setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));

		//Give some default values
		UserData newUser;
		newUser.clientSocket = clientSocket;
		newUser.textColor = CEGUI::Colour(1.0f, 1.0f, 1.0f, 1.0f);
		newUser.userName = "";

		// insert new client into session id table
		sessions.insert(std::pair<UserID, UserData>(outId, newUser) );

		return true;
	}

	return false;
}

void NetworkServer::RemoveClient(UserID id )
{
	//Search for client in map
	auto it = sessions.find(id);

	//If there is a client
	if(it != sessions.end())
	{
		//Remove it
		int result = closesocket(it->second.clientSocket);
		if(result == 0)
		{
			consoleWindow->PrintText("Client was successfully disconnected.", serverColour);
		}
		else
		{
			CEGUI::String errorMsg("Something went wrong when trying to disconnect client nr: " + it->first);
			errorMsg += ". Error code: " + WSAGetLastError();

			consoleWindow->PrintText(errorMsg, serverColour);
		}

		//No matter what we, we still erase the client.
		sessions.erase(it);
	}
}

void NetworkServer::SendDisconnectMessage(UserID client_id )
{
	//Search for client in map
	auto it = sessions.find(client_id);

	//If there is a client
	if(it != sessions.end())
	{
		std::string tempString(it->second.userName.c_str());
		tempString +=  + " has disconnected.";

		unsigned int stringSize = tempString.size();
		DataPacket outPacket;
		outPacket.dataType = TypeSTRING;
		outPacket.dataVector.resize(stringSize);
		memcpy(outPacket.dataVector.data(), tempString.data(), stringSize);

		dataToSend.push_back(std::move(outPacket));
	}
}


bool NetworkServer::Update()
{
	// get new clients
	if(AddClient(clientId))
	{
		consoleWindow->PrintText("Client has been connected to the server. Client ID is: " + std::to_string((unsigned long long)clientId), serverColour); 
		clientId++;
	}

	ReceiveClientData();

	//TODO
	SendDataToClients();

	return true;
}

bool NetworkServer::ReceiveClientData()
{
	int dataSize = 0;
	DataPacketType type = TypeNONE;

	for(auto iter = sessions.begin(); iter != sessions.end(); ++iter)
	{
		iFlag = 0;

		//TODO: redo this entire shit. Do it like client one

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
					clientsToDisconnect.push_back(iter);
					break;
				}

			default:
				{
					break;
				}
			}
		}
	}

	if(clientsToDisconnect.size() > 0)
	{
		//Potential cleanup of clients to be removed.
		for(auto iter = clientsToDisconnect.begin(); iter != clientsToDisconnect.end(); ++iter)
		{
			SendDisconnectMessage((*iter)->first);
			SendDisconnectMessage((*iter)->first);
			SendDisconnectMessage((*iter)->first);

			RemoveClient((*iter)->first);
		}

		//Then clear it.
		clientsToDisconnect.clear();
	}

	return true;
}

bool NetworkServer::SendDataToClients()
{
	//TODO: Do something with flag
	SOCKET currentSocket;
	int iSendResult;

	for(auto sessionIter = sessions.begin(); sessionIter != sessions.end(); ++sessionIter)
	{
		currentSocket = sessionIter->second.clientSocket;

		for(auto dataIter = dataToSend.begin(); dataIter != dataToSend.end(); ++dataIter)
		{
			//Create and serialize data header packet
			DataPacketHeader::Serialize(packet_header, dataIter->dataType, dataIter->dataVector.size());

			//Send data header
			iSendResult = NetworkServices::SendData(currentSocket, packet_header, DataPacketHeader::sizeOfStruct);

			if(iSendResult == SOCKET_ERROR) 
			{
				CEGUI::String errorMsg = "Failed to send data header to client nr: " + sessionIter->first;
				errorMsg += ". Error code: " + WSAGetLastError();

				consoleWindow->PrintText(errorMsg, serverColour);
				clientsToDisconnect.push_back(sessionIter);
			}

			//If we're actually sending a data packet and not just an event packet
			if(dataIter->dataVector.size() > 0)
			{
				//Send data body
				iSendResult = NetworkServices::SendData(currentSocket, dataIter->dataVector.data(), dataIter->dataVector.size());

				if(iSendResult == SOCKET_ERROR) 
				{
					CEGUI::String errorMsg = "Failed to send data to client nr: " + sessionIter->first;
					errorMsg += ". Error code: " + WSAGetLastError();

					consoleWindow->PrintText(errorMsg, serverColour);
					clientsToDisconnect.push_back(sessionIter);
				}
			}
		}	
	}

	dataToSend.clear();

	return true;
}

void NetworkServer::SendEventPacket(DataPacketType eventType)
{
	DataPacket packet;
	packet.dataType = eventType;

	dataToSend.push_back(packet);
}


void NetworkServer::ReadStringData(UserID client_id, char* receivingBuffer, int bufferSize)
{
	//TODO: send this back to all clients

	auto it = sessions.find(client_id);
	if(it != sessions.end())
	{
		DataPacket packet;

		//Set appropriate type for this packet
		packet.dataType = TypeCOLOUREDSTRING;

		//Save size of the username part of the string...
		unsigned int nameSize = it->second.userName.size();
		unsigned int textSize = bufferSize + 2 + nameSize;

		//Set vector to right size and init all values to default (0).
		//Adding 2 because I want to add a ": " between user name and whatever the user wrote.
		//And then finally, I want to append the user's text color
		packet.dataVector.resize(textSize + sizeof(CEGUI::argb_t));

		//Fill first part of data vector with the name of the user who sent this message
		memcpy(packet.dataVector.data(), it->second.userName.c_str(), nameSize);

		//Some rowdy cowboy coding to save performance.
		packet.dataVector[nameSize+0] = ':';
		packet.dataVector[nameSize+1] = ' ';
		nameSize += 2;

		auto argbVar = it->second.textColor.getARGB();

		//Read data and immediately put it into the packet
		NetworkServices::ReceiveData(it->second.clientSocket, packet.dataVector.data()+nameSize, bufferSize);

		memcpy(packet.dataVector.data()+textSize, &argbVar, sizeof(CEGUI::argb_t));

		//Store packet for redistribution
		dataToSend.push_back(std::move(packet));
	}
}


void NetworkServer::ReadUserData(UserID client_id, char* receivingBuffer, int bufferSize)
{
	auto it = sessions.find(client_id);
	if(it != sessions.end() )
	{
		//Read in the data in the form that was described by header
		NetworkServices::ReceiveData(it->second.clientSocket, receivingBuffer, bufferSize);

		auto& userData = it->second;		//read in data equal to bufferSize minus the colour part
		unsigned int calculatedTextSize = (bufferSize-sizeof(CEGUI::argb_t));
		std::vector<char> tempContainer(calculatedTextSize);
		CEGUI::argb_t tempColour;

		//First extract text from beginning of buffer
		memcpy(tempContainer.data(), receivingBuffer, calculatedTextSize);

		//Get colour appended to the end
		memcpy(&tempColour, receivingBuffer + calculatedTextSize, sizeof(CEGUI::argb_t));

		userData.textColor = CEGUI::Colour(tempColour);
		userData.userName = CEGUI::String(tempContainer.data(), calculatedTextSize);

		consoleWindow->PrintText(userData.userName + " has joined the server.", userData.textColor);
	}
}


bool NetworkServer::ReadDataHeader(UserID client_id, char* receivingBuffer, DataPacketType* outType, int* outSize )
{
	auto it = sessions.find(client_id);
	if(it != sessions.end() )
	{
		iResult = NetworkServices::ReceiveData(it->second.clientSocket, packet_header, DataPacketHeader::sizeOfStruct);
		if(iResult == 0)
		{
			consoleWindow->PrintText("Something went wrong when trying to read data packet header.", serverColour);
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