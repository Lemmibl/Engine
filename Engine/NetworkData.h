#define WIN32_LEAN_AND_MEAN

#pragma once
#include <string.h>
#include <CEGUI/CEGUI.h>

#define MAX_PACKET_SIZE 4096

struct UserData
{
	SOCKET clientSocket;
	CEGUI::String userName;
	CEGUI::Colour textColor;
};

enum PacketTypes 
{
	INIT_CONNECTION = 0,
	ACTION_EVENT = 1,
	DISCONNECT_EVENT = 2
}; 

enum DataPacketType
{
	TypeNONE = 0,
	TypeSTRING = 1,
	TypeFLOAT = 2,
	TypeUSERDATA = 3,
	TypeDISCONNECT = 4
};

//This datapacket is supposed to be a precursor to sending the real data.
//It will detail the data type and size of the incoming packet
// http://stackoverflow.com/questions/5795283/creating-and-sending-data-packets-in-c-c
struct DataPacketHeader
{
	//Reserving the first two bytes for dataType and the last two bytes for size of next packet
	static const size_t sizeOfStruct = sizeof(char)*4;

	//char* outData NEEDS to be 6 bytes long
	static void Serialize(char outData[4], DataPacketType type, unsigned int size) 
	{
		//Prepare data
		short tempType = htons(type);
		short tempSize = htons(size);

		//Store type in first two bytes
		memcpy(outData, &tempType, 2);

		//Store size in the rest of the bytes
		memcpy(outData+2, &tempSize, 2);
	}

	//char* outData NEEDS to be 6 bytes long
	static void Deserialize(char inData[4], DataPacketType* outType, unsigned int* outSize) 
	{
		//Read DataPacketType from first two bytes
		memcpy(outType, inData+0, 2);
		
		//Convert it using appropriate function.
		*outType = static_cast<DataPacketType>(ntohs(*outType));

		//Read the size of the next packet that we'll receive from the next two bytes.
		memcpy(outSize, inData+2, 2);

		//Convert data using appropriate function.
		*outSize = ntohs(*outSize);
	}
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