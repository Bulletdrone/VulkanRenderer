#include "RenderShape.h"

RenderShape::RenderShape(const size_t a_ID, Transform* a_Transform, PipeLineData* a_PipeLineData, RenderObjectData* a_RenderObjectData)
	:	BaseRenderObject(a_ID, a_Transform, a_PipeLineData)
{
	p_RenderObjectData= a_RenderObjectData;
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