#include "GeometryFactory.h"

#include "Renderer.h"
#include "RenderObjects/Components/MeshData.h"

void GeometryFactory::Init(Renderer* a_Renderer)
{
	{
		//Generate Triangle Mesh
		std::vector<Vertex> t_Vertices =
		{ {{0.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },
			{{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}} };

		std::vector<uint32_t> t_Indices = { 0, 1, 2 };

		m_ShapeHandlers[static_cast<size_t>(GeometryType::Triangle)] = a_Renderer->GenerateMesh(t_Vertices, t_Indices);
	}

	{
		//Generate QuadMesh
		std::vector<Vertex> t_Vertices =
		{ {{-1.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
			{{1.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
			{{-1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}} };

		std::vector<uint32_t> t_Indices = { 0, 1, 2, 2, 3, 0 };

		m_ShapeHandlers[static_cast<size_t>(GeometryType::Quad)] = a_Renderer->GenerateMesh(t_Vertices, t_Indices);
	}
}

uint32_t GeometryFactory::GetShape(GeometryType a_Type)
{
	return m_ShapeHandlers[static_cast<size_t>(a_Type)];
}
