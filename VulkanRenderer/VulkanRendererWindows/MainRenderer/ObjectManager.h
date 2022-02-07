#pragma once

#include "Renderer.h"
#include "RenderObjects/Geometry/GeometryFactory.h"

class BaseRenderObject;

class ObjectManager
{
public:
	ObjectManager(Renderer* a_Renderer);
	~ObjectManager();

	void SetupStartObjects();

	void UpdateObjects(float a_Dt);

	//Create a renderobject from a basic shape.
	BaseRenderObject* CreateRenderObject(Transform* a_Transform, Material& a_Material, GeometryType a_Type);
	//Create a renderobject from a mesh.
	BaseRenderObject* CreateRenderObject(Transform* a_Transform, Material& a_Material, const char* a_MeshPath);

	//Increases the m_CurrentRenderID by 1 and returns it.
	uint32_t GetNextRenderID() { return m_CurrentRenderID++; }

private:
	uint32_t m_CurrentRenderID = 0;

	std::vector<BaseRenderObject*> m_RenderObjects;

	DescriptorAllocator* m_DescriptorAllocator;
	DescriptorLayoutCache* m_DescriptorLayoutCache;

	GeometryFactory m_GeometryFactory;

public:
	//Data pipeline
	Material mat_Pavillion;
	Material mat_Rectangle;

	//Data descriptor
	VkDescriptorSetLayout layout_SingleBufferCamera;
	VkDescriptorSetLayout layout_SingleImageObject;

	Renderer* p_Renderer;
};