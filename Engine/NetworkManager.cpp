#include "NetworkManager.h"
#include "GameConsoleWindow.h"

NetworkManager::NetworkManager()
{
}


NetworkManager::~NetworkManager()
{
}

bool NetworkManager::Connect(GameConsoleWindow* console, std::string ip, std::string port)
{
	WSADATA wsaData;
	SOCKET connectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;
	char* serverName = nullptr;


	/*
	 To execute the client, compile the complete client source code and run the executable file. 
	 The client application requires that name of the computer or IP address of the computer where the server application 
	 is running is passed as a command-line parameter when the client is executed. 
	 If the client and server are executed on the sample computer, the client can be started as follows:
	 */

	console->PrintText("Now trying to connect to IP: " + ip + " Port: " + port);

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iResult < 0) 
	{
		console->PrintText("WSAStartup failed. Error code:" + iResult);
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
		console->PrintText("getaddrinfo failed. Error code: " + iResult);
		WSACleanup();
		return false;
	}

	// Attempt to connect to an address until one succeeds
	for(ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (connectSocket == INVALID_SOCKET) 
		{
			console->PrintText("Socket failed with error code: " + WSAGetLastError());
			WSACleanup();
			return false;
		}

		// Connect to server.
		iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) 
		{
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (connectSocket == INVALID_SOCKET) 
	{
		console->PrintText("Unable to connect to server!");
		WSACleanup();
		return false;
	}

	// Send an initial buffer
	iResult = send( connectSocket, sendbuf, (int)strlen(sendbuf), 0 );
	if (iResult == SOCKET_ERROR) {
		console->PrintText("Send failed with error: " + WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();

		return false;
	}

	console->PrintText("Bytes Sent: " + iResult);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(connectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) 
	{
		console->PrintText("Shutdown failed with error: " + WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return false;
	}

	auto start = std::clock();

	// Receive until the peer closes the connection
	do 
	{
		iResult = recv(connectSocket, recvbuf, recvbuflen, 0);

		if (iResult > 0 )
		{
			console->PrintText("Bytes received: " + iResult);
		}
		else if ( iResult == 0 )
		{
			console->PrintText("Connection closed.");
		}
		else
		{
			console->PrintText("recv failed with error: " + WSAGetLastError());
		}
	} while( iResult > 0 || std::difftime(std::clock(), start) > 10.0f); //If we get an error OR 10 seconds has passed, we break the while loop.

	// cleanup
	closesocket(connectSocket);
	WSACleanup();

	return true;
}
