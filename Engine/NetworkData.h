#define WIN32_LEAN_AND_MEAN

#pragma once
#include <string.h>

#define MAX_PACKET_SIZE 4096

enum PacketTypes 
{
	INIT_CONNECTION = 0,
	ACTION_EVENT = 1,
	DISCONNECT_EVENT = 2
}; 

struct EventPacket 
{
	unsigned int packet_type;

	void Serialize(char* data) 
	{
		memcpy(data, this, sizeof(EventPacket));
	}

	void Deserialize(char* data) 
	{
		memcpy(this, data, sizeof(EventPacket));
	}
};

//struct StringPacket
//{
//	unsigned int stringSize;
//	char* stringData;
//
//	void Serialize(unsigned int size, char* data) 
//	{
//		stringSize = size;
//		memcpy(data, stringData, sizeof(char)*size);
//	}
//
//	unsigned int Deserialize(char* data) 
//	{
//		memcpy(&stringSize, data, sizeof(unsigned int));
//							//Offset by size of one unsigned int
//		memcpy(&stringData, data+sizeof(unsigned int), sizeof(char)*(stringSize-1));
//
//		return stringSize;
//	}
//};