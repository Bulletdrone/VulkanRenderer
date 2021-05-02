#pragma once

#include "Renderer.h"

class ObjectManager
{
public:
	ObjectManager(Renderer* a_Renderer);
	~ObjectManager();

	void UpdateObjects(float a_Dt);
	void SetupRenderObjects();

private:
	std::vector<MeshData*> m_RenderObjects;

	Renderer* p_Renderer;
};

