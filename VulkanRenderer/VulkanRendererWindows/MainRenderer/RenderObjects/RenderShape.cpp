#include "RenderShape.h"

RenderShape::RenderShape(const uint32_t a_ID, Transform* a_Transform, RenderHandle a_MatHandle, RenderHandle a_MeshHandle)
	:	BaseRenderObject(a_ID, a_Transform, a_MatHandle)
{
	m_MeshHandle = a_MeshHandle;
}

RenderShape::~RenderShape()
{}

void RenderShape::Update()
{
	//Rotation().x += 0.01f;

	UpdateModelMatrix();
}

void RenderShape::Draw()
{
}
