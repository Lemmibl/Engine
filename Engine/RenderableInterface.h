#pragma once
#include <d3d11.h>

class RenderableInterface
{
public:
	virtual bool Render(ID3D11DeviceContext* deviceContext) = 0;
};

