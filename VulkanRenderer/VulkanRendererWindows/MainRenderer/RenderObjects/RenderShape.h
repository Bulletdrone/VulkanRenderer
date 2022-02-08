#pragma once
#include "BaseRenderObject.h"

class RenderShape : public BaseRenderObject
{
public:
	RenderShape(const uint32_t a_ID, Transform* a_Transform, uint32_t a_MatHandle, MeshData* a_MeshData);
	virtual ~RenderShape();

	void Update() override;
	void Draw() override;

private:

};

