#include "ObjectManager.h"

ObjectManager::ObjectManager(Renderer* a_Renderer)
	:	p_Renderer(a_Renderer)
{
	std::vector<Vertex> t0_Vertices = 
	{
		{{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}
	};
	const std::vector<uint16_t> t0_Indices = 
	{
		0, 1, 2, 2, 3, 0
	};

	MeshData* t0 = new MeshData(t0_Vertices, t0_Indices);
	m_RenderObjects.push_back(t0);

	std::vector<Vertex> t1_Vertices =
	{
		{{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	};
	const std::vector<uint16_t> t1_Indices =
	{
		0, 1, 2
	};

	MeshData* t1 = new MeshData(t1_Vertices, t1_Indices);
	m_RenderObjects.push_back(t1);

	p_Renderer->SetRenderObjectsVector(&m_RenderObjects);
}

ObjectManager::~ObjectManager()
{
}

void ObjectManager::UpdateObjects(float a_Dt)
{
	uint32_t t_ImageIndex;
	p_Renderer->DrawFrame(t_ImageIndex);
}

void ObjectManager::SetupRenderObjects()
{
	for (size_t i = 0; i < m_RenderObjects.size(); i++)
	{
		p_Renderer->SetupMesh(m_RenderObjects[i]);
	}
}
