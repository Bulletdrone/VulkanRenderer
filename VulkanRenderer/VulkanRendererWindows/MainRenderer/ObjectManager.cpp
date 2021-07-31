#include "ObjectManager.h"

ObjectManager::ObjectManager(Renderer* a_Renderer)
	: p_Renderer(a_Renderer)
{
	p_Renderer->SetRenderObjectsVector(&m_RenderObjects);
	p_Renderer->SetTextureDataVector(&m_Textures);

	m_RenderFactory = new RenderFactory();

	m_Textures.resize(1);

	//pip_SpaceImage.p_DescriptorData = &m_DescriptorData[0];
	//pip_SpaceImage.p_DescriptorData->texture = &m_Textures[0];
	//Load Texture
	des_SpaceImage = p_Renderer->CreateDescriptorLayout(&m_Textures[0]);
	pip_SpaceImage = p_Renderer->CreateGraphicsPipeline(des_SpaceImage);
	p_Renderer->SetupImage(m_Textures[0], "../Resources/Images/Background.png");
}

ObjectManager::~ObjectManager()
{
	for (size_t i = 0; i < m_RenderObjects.size(); i++)
	{
		m_RenderObjects[i]->GetMeshData()->DeleteBuffers(p_Renderer->GetVulkanDevice().m_LogicalDevice);
		delete m_RenderObjects[i];
	}
	m_RenderObjects.clear();
}

void ObjectManager::SetupStartObjects()
{
	p_Renderer->SetupRenderObjects();
	p_Renderer->CreateDescriptorPool(des_SpaceImage);
	p_Renderer->CreateDescriptorSet(des_SpaceImage);
}

void ObjectManager::UpdateObjects(float a_Dt)
{
	uint32_t t_ImageIndex;

	for (size_t i = 0; i < m_RenderObjects.size(); i++)
	{
		m_RenderObjects[i]->Update();
	}

	p_Renderer->DrawFrame(t_ImageIndex, a_Dt);

	//Update Uniform Buffer
	//p_Renderer->UpdateUniformBuffer(t_ImageIndex, a_Dt);
}

void ObjectManager::CreateShape(ShapeType a_ShapeType, Transform* a_Transform)
{
	BaseRenderObject* t_NewShape = m_RenderFactory->CreateRenderObject(GetNextRenderID(), a_ShapeType, a_Transform, pip_SpaceImage);
	m_RenderObjects.push_back(t_NewShape);
	
	p_Renderer->SetupMesh(t_NewShape->GetMeshData());
}
