#include "RenderFactory.h"

RenderFactory::RenderFactory()
{
	//std::vector<ShapeType> t_Shape;
	//t_Shape.push_back(ShapeType::Triangle);
	//t_Shape.push_back(ShapeType::Rectangle);
	//t_Shape.push_back(ShapeType::Pavillion);
	m_RenderObjectsData.resize(m_AllShapes);

	//SetSceneObjects(t_Shape);
}

RenderFactory::~RenderFactory()
{}

BaseRenderObject* RenderFactory::CreateRenderObject(const uint32_t a_RenderID, Transform* a_Transform, Material& a_Material)
{
	return new RenderShape(a_RenderID, a_Transform, a_Material, m_RenderObjectsData[static_cast<size_t>(a_ShapeType)]);
}

void RenderFactory::CreateRenderObject()
{
	//MeshData* t_Mesh;

	////temp
	//std::vector<Vertex> t_Vertices;
	//std::vector<uint32_t> t_Indices;

	//switch (a_ShapeType)
	//{
	//default:
	//	t_Mesh = nullptr;
	//	break;
	//case ShapeType::Triangle:
	//	t_Vertices = { {{0.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },
	//		{{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
	//		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
	//	};

	//	t_Indices = { 0, 1, 2 };

	//	t_Mesh = new MeshData();
	//	t_Mesh->vertices = new BufferData<Vertex>(t_Vertices);
	//	t_Mesh->indices = new BufferData<uint32_t>(t_Indices);

	//	break;
	//case ShapeType::Rectangle:
	//	t_Vertices = { {{-1.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
	//					{{1.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
	//					{{1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
	//					{{-1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}} };

	//	t_Indices = { 0, 1, 2, 2, 3, 0 };

	//	t_Mesh = new MeshData();
	//	t_Mesh->vertices = new BufferData<Vertex>(t_Vertices);
	//	t_Mesh->indices = new BufferData<uint32_t>(t_Indices);

	//	break;
	//case ShapeType::SkyBoxRect:
	//	throw;
	//	break;
	//case ShapeType::Pavillion:
	//	t_Mesh = ResourceLoader::LoadModel("../VulkanRenderer/Resources/Models/Pavillion.obj");
	//	break;
	//}
}
