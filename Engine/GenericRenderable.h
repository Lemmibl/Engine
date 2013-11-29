#pragma once
enum RenderableType
{
	VertexOnly,
	Indexed,
	InstancedVertexOnly,
	IndexedInstanced
};

class GenericRenderable
{
public:
	GenericRenderable(RenderableType type);
	virtual ~GenericRenderable();

	const RenderableType GetRenderableType() const { return renderableType; }

private:
	const RenderableType renderableType;
};

