#pragma once
#include <d3d11.h>

class RenderableInterface
{
public:
	virtual void Render(ID3D11DeviceContext* deviceContext) = 0;
};

