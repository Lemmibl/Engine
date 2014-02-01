#pragma once
class FullScreenShaderInterface
{
private:
	//Literally empty
	struct EmptyVertex
	{
	};

public:
	FullScreenShaderInterface();
	~FullScreenShaderInterface();

	//Just three literally empty vertices, used for the vertex shader
	EmptyVertex emptyVertices[3];
};

