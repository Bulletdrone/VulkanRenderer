#pragma once

#include "Renderer.h"
#include "RenderObjects/RenderFactory.h"

class ObjectManager
{
public:
	ObjectManager(Renderer* a_Renderer);
	~ObjectManager();

	void UpdateObjects(float a_Dt);
	void CreateShape(ShapeType a_ShapeType, Transform* a_Transform);


	//Increases the m_CurrentRenderID by 1 and returns it.
	size_t GetNextRenderID() { return m_CurrentRenderID++; }

private:

	size_t m_CurrentRenderID;

	std::vector<BaseRenderObject*> m_RenderObjects;
	std::vector<MeshData*> m_PointerToMeshes;

	Renderer* p_Renderer;
};

