#include "NetworkServices.h"

int NetworkServices::SendData(const SOCKET curSocket, const char* sendingBuffer, const int bufferSize, const int flag)
{
	return send(curSocket, sendingBuffer, bufferSize, flag);
}

int NetworkServices::ReceiveData(SOCKET curSocket, char* receivingBuffer, int bufferSize, int flag)
{
	return recv(curSocket, receivingBuffer, bufferSize, flag);
}