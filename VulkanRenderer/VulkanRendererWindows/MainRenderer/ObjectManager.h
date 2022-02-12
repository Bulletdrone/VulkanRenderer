#pragma once

#include "Renderer.h"
#include "RenderObjects/Geometry/GeometryFactory.h"

class BaseRenderObject;

class ObjectManager
{
public:
	ObjectManager(Renderer* a_Renderer);
	~ObjectManager();

	void UpdateObjects(float a_Dt);

	//Create a renderobject from a basic shape.
	BaseRenderObject* CreateRenderObject(Transform* a_Transform, MaterialHandle a_MaterialHandle, GeometryType a_Type);
	//Create a renderobject from a mesh.
	BaseRenderObject* CreateRenderObject(Transform* a_Transform, MaterialHandle a_MaterialHandle, MeshHandle a_MeshHandle);

	//Increases the m_CurrentRenderID by 1 and returns it.
	uint32_t GetNextRenderID() { return m_CurrentRenderID++; }

private:
	uint32_t m_CurrentRenderID = 0;

	std::vector<BaseRenderObject*> m_RenderObjects;

	GeometryFactory m_GeometryFactory;

public:
	Renderer* p_Renderer;
};