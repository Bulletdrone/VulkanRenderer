#include "RenderFactory.h"

BaseRenderObject* RenderFactory::CreateShape(const size_t a_RenderID, ShapeType a_ShapeType, Transform* a_Transform)
{
	std::vector<Vertex> t_Vertices;
	std::vector<uint16_t> t_Indices;

	switch (a_ShapeType)
	{
	case ShapeType::Triangle:
		t_Vertices = {	{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
						{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
						{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, };

		t_Indices = { 0, 1, 2 };	 
		break;
	case ShapeType::Rectangle:
		t_Vertices = {	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
						{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
						{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
						{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}} };

		t_Indices = { 0, 1, 2, 2, 3, 0 };
		break;
	}

	MeshData* shapeMesh = new MeshData(t_Vertices, t_Indices);

	return new RenderShape(a_RenderID, a_Transform, shapeMesh);
}
