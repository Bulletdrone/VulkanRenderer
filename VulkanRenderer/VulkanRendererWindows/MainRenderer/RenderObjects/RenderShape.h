#pragma once
#include "BaseRenderObject.h"

class RenderShape : public BaseRenderObject
{
public:
	RenderShape(const size_t a_ID, Transform* a_Transform, MeshData* a_MeshData); // Create Object with a prebuild Meshdata.
	~RenderShape();

	void Update() override;
	void Draw() override;

private:

};

