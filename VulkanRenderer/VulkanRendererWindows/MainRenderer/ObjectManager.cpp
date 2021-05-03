#include "ObjectManager.h"

ObjectManager::ObjectManager(Renderer* a_Renderer)
	:	p_Renderer(a_Renderer)
{
	p_Renderer->SetRenderObjectsVector(&m_PointerToMeshes);
}

ObjectManager::~ObjectManager()
{
	for (size_t i = 0; i < m_RenderObjects.size(); i++)
	{
		m_RenderObjects[i]->GetMeshData()->DeleteBuffers(p_Renderer->GetLogicalDevice());
		delete m_RenderObjects[i];
	}
	m_RenderObjects.clear();
	m_PointerToMeshes.clear();
}

void ObjectManager::UpdateObjects(float a_Dt)
{
	uint32_t t_ImageIndex;
	p_Renderer->DrawFrame(t_ImageIndex, a_Dt);

	//Update Uniform Buffer
	p_Renderer->UpdateUniformBuffer(t_ImageIndex, a_Dt);
}

void ObjectManager::CreateShape(ShapeType a_ShapeType, Transform* a_Transform)
{
	BaseRenderObject* t_NewShape = RenderFactory::CreateShape(GetNextRenderID(), a_ShapeType, a_Transform);
	m_RenderObjects.push_back(t_NewShape);

	//Push the MeshData towards the Renderer as well.
	m_PointerToMeshes.push_back(t_NewShape->GetMeshData());
	
	p_Renderer->SetupMesh(t_NewShape->GetMeshData());
}
