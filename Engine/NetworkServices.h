#define WIN32_LEAN_AND_MEAN
#pragma once
#include <WinSock2.h>
#include <windows.h>

class NetworkServices
{
public:
	static int SendMessage(SOCKET currentSocket, const char* sendingBuffer, int bufferSize, int flags = 0);
	static int ReceiveMessage(SOCKET currentSocket, char* receivingBuffer, int bufferSize, int& inFlags);
};

