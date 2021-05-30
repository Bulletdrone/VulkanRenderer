#include "ObjectManager.h"

ObjectManager::ObjectManager(Renderer* a_Renderer)
	: p_Renderer(a_Renderer)
{
	p_Renderer->SetRenderObjectsVector(&m_RenderObjects);
	p_Renderer->SetTextureDataVector(&m_Textures);

	m_RenderFactory = new RenderFactory();

	m_Textures.resize(1);
	m_PipeLineData.resize(1);
	m_DescriptorData.resize(1);

	m_PipeLineData[0].p_DescriptorData = &m_DescriptorData[0];
	m_PipeLineData[0].p_DescriptorData->texture = &m_Textures[0];
	//Load Texture
	p_Renderer->CreateDescriptorLayout(m_DescriptorData[0]);
	p_Renderer->CreateGraphicsPipeline(m_PipeLineData[0]);
	p_Renderer->SetupImage(m_Textures[0], "../Resources/Images/Background.png");
}

ObjectManager::~ObjectManager()
{
	for (size_t i = 0; i < m_RenderObjects.size(); i++)
	{
		m_RenderObjects[i]->GetMeshData()->DeleteBuffers(p_Renderer->GetLogicalDevice());
		delete m_RenderObjects[i];
	}
	m_RenderObjects.clear();
}

void ObjectManager::SetupStartObjects()
{
	p_Renderer->SetupRenderObjects();
	p_Renderer->CreateDescriptorPool(m_DescriptorData[0]);
	for (size_t i = 0; i < m_RenderObjects.size(); i++)
	{
		p_Renderer->CreateDescriptorSet(m_RenderObjects[i]);
	}
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
	BaseRenderObject* t_NewShape = m_RenderFactory->CreateRenderObject(GetNextRenderID(), a_ShapeType, a_Transform, &m_PipeLineData.at(0));
	m_RenderObjects.push_back(t_NewShape);
	
	p_Renderer->SetupMesh(t_NewShape->GetMeshData());
}
