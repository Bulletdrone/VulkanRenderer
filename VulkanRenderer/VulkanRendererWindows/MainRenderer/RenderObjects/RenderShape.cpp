#include "RenderShape.h"

RenderShape::RenderShape(const uint32_t a_ID, Transform* a_Transform, Material& a_Material, MeshData* a_MeshData)
	:	BaseRenderObject(a_ID, a_Transform, a_Material)
{
	p_MeshData = a_MeshData;
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
