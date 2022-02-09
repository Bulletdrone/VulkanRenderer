#include "ObjectManager.h"
#include "Tools/VulkanInitializers.h"

#include "RenderObjects/RenderShape.h"

ObjectManager::ObjectManager(Renderer* a_Renderer)
	: p_Renderer(a_Renderer)
{
	m_GeometryFactory.Init(p_Renderer);

	p_Renderer->SetRenderObjectsVector(&m_RenderObjects);
}

ObjectManager::~ObjectManager()
{
	for (size_t i = 0; i < m_RenderObjects.size(); i++)
	{
		//m_RenderObjects[i]->GetMeshData()->DeleteBuffers(p_Renderer->GetVulkanDevice().m_LogicalDevice);
		delete m_RenderObjects[i];
	}
	m_RenderObjects.clear();
}

void ObjectManager::UpdateObjects(float a_Dt)
{
	(void)a_Dt;
	uint32_t t_ImageIndex;

	for (size_t i = 0; i < m_RenderObjects.size(); i++)
	{
		m_RenderObjects[i]->Update();
	}

	p_Renderer->DrawFrame(t_ImageIndex);
}

BaseRenderObject* ObjectManager::CreateRenderObject(Transform* a_Transform, RenderHandle a_MaterialHandle, GeometryType a_Type)
{
	RenderShape* t_Shape = new RenderShape(GetNextRenderID(), a_Transform, a_MaterialHandle, m_GeometryFactory.GetShape(a_Type));

	m_RenderObjects.push_back(t_Shape);

	return t_Shape;
}

BaseRenderObject* ObjectManager::CreateRenderObject(Transform* a_Transform, RenderHandle a_MaterialHandle, RenderHandle a_MeshHandle)
{
	RenderShape* t_Shape = new RenderShape(GetNextRenderID(), a_Transform, a_MaterialHandle, a_MeshHandle);

	m_RenderObjects.push_back(t_Shape);

	return t_Shape;
}