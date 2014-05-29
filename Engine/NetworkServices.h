#define WIN32_LEAN_AND_MEAN
#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <cstdint>

class NetworkServices
{
public:
	static int SendData(const SOCKET currentSocket, const char* sendingBuffer, const int bufferSize, const int flag = 0);
	static int ReceiveData(SOCKET currentSocket, char* receivingBuffer, const int bufferSize, const int flag = 0);
};