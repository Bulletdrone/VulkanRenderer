#pragma once
#include "BaseRenderObject.h"

class RenderShape : public BaseRenderObject
{
public:
	RenderShape(const uint32_t a_ID, Transform* a_Transform, MaterialHandle a_MatHandle, MeshHandle a_MeshHandle);
	virtual ~RenderShape();

	void Update() override;
	void Draw() override;

private:

};

