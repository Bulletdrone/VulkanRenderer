#include "GeometryFactory.h"

#include "Renderer.h"
#include "RenderObjects/Components/MeshData.h"

void GeometryFactory::Init(Renderer* a_Renderer)
{
	m_ShapeData.resize(static_cast<size_t>(GeometryType::Count));

	{
		//Generate Triangle Mesh
		MeshData* t_TriangleMesh = new MeshData;

		std::vector<Vertex> t_Vertices =
		{ {{0.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },
			{{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}} };

		std::vector<uint32_t> t_Indices = { 0, 1, 2 };

		t_TriangleMesh->vertices = new BufferData<Vertex>(t_Vertices);
		t_TriangleMesh->indices = new BufferData<uint32_t>(t_Indices);

		a_Renderer->SetupMesh(t_TriangleMesh);

		m_ShapeData.insert(m_ShapeData.begin() + static_cast<size_t>(GeometryType::Triangle),
			t_TriangleMesh);
	}

	{
		//Generate QuadMesh
		MeshData* t_QuadMesh = new MeshData;

		std::vector<Vertex> t_Vertices =
		{ {{-1.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
			{{1.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
			{{-1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}} };

		std::vector<uint32_t> t_Indices = { 0, 1, 2, 2, 3, 0 };

		t_QuadMesh->vertices = new BufferData<Vertex>(t_Vertices);
		t_QuadMesh->indices = new BufferData<uint32_t>(t_Indices);

		a_Renderer->SetupMesh(t_QuadMesh);

		m_ShapeData.insert(m_ShapeData.begin() + static_cast<size_t>(GeometryType::Quad),
			t_QuadMesh);
	}
}

void GeometryFactory::Cleanup(VkDevice& a_VkDevice)
{
	for (auto& shape : m_ShapeData)
	{
		shape->DeleteBuffers(a_VkDevice);
	}

	m_ShapeData.clear();
}

MeshData* GeometryFactory::GetShape(GeometryType a_Type)
{
	return m_ShapeData[static_cast<size_t>(a_Type)];
}
