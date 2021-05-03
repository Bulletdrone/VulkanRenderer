#include "RenderShape.h"

RenderShape::RenderShape(const size_t a_ID, Transform* a_Transform, MeshData* a_MeshData)
	:	BaseRenderObject(a_ID, a_Transform)
{
	m_MeshData = a_MeshData;
}

RenderShape::~RenderShape()
{
}

void RenderShape::Update()
{
}

void RenderShape::Draw()
{
}
