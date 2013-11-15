#pragma once
#include <queue>
#include <d3d11.h>
#include <atlcomcli.h>
#include <xnamath.h>
#include <string>
#include "fontclass.h"

class SentenceManager
{
public:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	struct SentenceType
	{
		bool isActive;
		CComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;
		int vertexCount, indexCount, maxLength;
		XMFLOAT4 ColorRGBA;
	};

public:
	SentenceManager(ID3D11Device* device, unsigned int maxSentences, int screenWidth, int screenHeight);
	~SentenceManager();

	int CreateSentence(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int maxLength, std::string textString, int positionX, int positionY, XMFLOAT3 rgbColor);
	bool UpdateSentence(ID3D11DeviceContext* deviceContext, int id, std::string textString, int positionX, int positionY, XMFLOAT3 rgbColor);
	void Reset(); //Releases all sentences


	void ReleaseSentence(int id) { sentences[id].isActive = false;  prioQueue.push(id); };
	const SentenceType* GetSpecificSentence(int id) const { return &sentences[id]; };
	const std::vector<SentenceType>* GetAllSentences() const { return &sentences; };
	FontClass* GetFont() { return (&font); };

private:
	int GetNextSentenceSlot() { int result = prioQueue.top(); prioQueue.pop(); return result; }

private:
	std::priority_queue<int> prioQueue;
	std::vector<SentenceType> sentences;
	const int screenWidth, screenHeight;
	unsigned int maxSentences;
	FontClass font;
};

