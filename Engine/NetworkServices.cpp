#include "NetworkServices.h"

int NetworkServices::SendMessage(SOCKET curSocket, const char* sendingBuffer, int bufferSize, int flags)
{
	return send(curSocket, sendingBuffer, bufferSize, flags);
}

int NetworkServices::ReceiveMessage(SOCKET curSocket, char* receivingBuffer, int bufferSize, int& inFlags)
{
	return recv(curSocket, receivingBuffer, bufferSize, inFlags);
}