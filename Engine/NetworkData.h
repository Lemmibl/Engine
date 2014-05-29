#define WIN32_LEAN_AND_MEAN

#pragma once
#include <string.h>
#include <CEGUI/CEGUI.h>

#define MAX_PACKET_SIZE 1024

struct UserData
{
	SOCKET clientSocket;
	CEGUI::String userName;
	CEGUI::Colour textColor;
};

enum DataPacketType
{
	TypeNONE = 0,
	TypeSTRING,
	TypeCOLOUREDSTRING,
	TypeFLOAT,
	TypeUSERDATA,
	TypeCONNECT,
	TypeDISCONNECT
};

//Struct to contain the actual data that will be sent after a DataPacketHeader
struct DataPacket
{
	DataPacket()
	: dataVector(0),
	dataType(TypeNONE)
	{
	}

	//Might as well keep it in a vector. Also contains size of the data that we're going to send through size().
	std::vector<char> dataVector;
	DataPacketType dataType;
};

//This datapacket is supposed to be a precursor to sending the real data.
//It will detail the data type and size of the incoming packet
// http://stackoverflow.com/questions/5795283/creating-and-sending-data-packets-in-c-c
struct DataPacketHeader
{
	//Reserving the first two bytes for dataType and the last two bytes for size of next packet
	static const size_t sizeOfStruct = sizeof(char)*4;

	//char* outData NEEDS to be 6 bytes long
	static void Serialize(char outData[sizeOfStruct], DataPacketType type, int size) 
	{
		//Prepare data
		unsigned short tempType = htons(type);
		unsigned short tempSize = htons(size);

		//Store type in first two bytes
		memcpy(outData, &tempType, 2);

		//Store size in the rest of the bytes
		memcpy(outData+2, &tempSize, 2);
	}

	//char* outData NEEDS to be 6 bytes long
	static void Deserialize(char inData[4], DataPacketType* outType, int* outSize) 
	{
		//Read DataPacketType from first two bytes
		memcpy(outType, inData+0, 2);
		
		//Convert it using appropriate function.
		*outType = static_cast<DataPacketType>(ntohs(*outType));

		//Read the size of the next packet that we'll receive from the next two bytes.
		memcpy(outSize, inData+2, 2);

		//Convert data using appropriate function.
		//Cast it up from ushort to uint. Not really needed but I use uints outside so might as well implicitly cast it.
		*outSize = ntohs(*outSize);
	}
};