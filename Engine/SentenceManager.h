#pragma once
#include <queue>
#include <d3d11.h>
#include <atlcomcli.h>
#include <xnamath.h>
#include <string>
#include "fontclass.h"

//http://en.wikipedia.org/wiki/65535_(number)   ... :)
//Compiler bitches about redefinition
//#define UINT16_MAX 65535
const unsigned short MaxSentences = 65534; //so max-1. The max value is designated as an "EMPTY" marker

class SentenceManager
{
public:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	struct Handle
	{
		unsigned short id;
		unsigned short generation;
	};

	struct SentenceType
	{
		CComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;
		int vertexCount, indexCount, maxLength;
		XMFLOAT4 ColorRGBA;
		unsigned short id;
	};

public:
	SentenceManager(ID3D11Device* device, int screenWidth, int screenHeight);
	~SentenceManager();

	Handle CreateSentence(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int maxLength, std::string textString, int positionX, int positionY, XMFLOAT3 rgbColor);
	bool UpdateSentence(ID3D11DeviceContext* deviceContext, Handle handle, std::string textString, int positionX, int positionY, XMFLOAT3 rgbColor);
	void Reset(); //Releases all sentences


	void ReleaseSentence(int id);
	bool IsSentenceValid(Handle handle);
	const SentenceType& GetSpecificSentence(Handle handle) const { return sentences[indices[handle.id].index]; };
	const SentenceType* GetAllSentences() const { return sentences; };
	unsigned short GetActiveSentenceCount() { return activeSentenceCount; };

	void SetScreenWidth(int width) { screenWidth = width; }
	void SetScreenHeight(int height) { screenHeight = height; }
	FontClass* GetFont() { return &font; };

private:
	struct Index
	{
		unsigned short index;
		unsigned short generation;
	};

private:
	int GetNextSentenceSlot() { int result = prioQueue.top(); prioQueue.pop(); return result; }

private:
	std::priority_queue<unsigned short, vector<unsigned short>, std::greater<unsigned short>> prioQueue;

	SentenceType sentences[MaxSentences];
	Index indices[MaxSentences];

	int screenWidth, screenHeight;
	unsigned short activeSentenceCount;
	FontClass font;
};

