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
{
	ClearRenderObjects();
}

BaseRenderObject* RenderFactory::CreateRenderObject(bool& r_NewObject, const uint32_t a_RenderID, ShapeType a_ShapeType, Transform* a_Transform, Material& a_Material)
{
	CheckRenderObject(a_ShapeType);
	return new RenderShape(a_RenderID, a_Transform, a_Material, m_RenderObjectsData[static_cast<size_t>(a_ShapeType)]);
}

//Clear all the current loaded objects.
void RenderFactory::ClearRenderObjects()
{
	for (size_t i = 0; i < m_CurrentLoadedObjectsID.size(); i++)
	{
		delete m_RenderObjectsData[m_CurrentLoadedObjectsID[i]];
	}
	m_RenderObjectsData.clear();
	m_CurrentLoadedObjectsID.clear();
}

bool RenderFactory::CheckRenderObject(ShapeType a_ShapeType)
{
	for (size_t i = 0; i < m_CurrentLoadedObjectsID.size(); i++)
	{
		if (m_CurrentLoadedObjectsID[i] == static_cast<uint32_t>(a_ShapeType))
			return false;
	}

	CreateRenderObject(a_ShapeType);
	return true;
}

void RenderFactory::ResetRenderObjects(const std::vector<uint32_t>& a_AvailableRenderObjects)
{
	ClearRenderObjects();
	m_RenderObjectsData.resize(m_AllShapes);

	for (size_t i = 0; i < a_AvailableRenderObjects.size(); i++)
	{
		CreateRenderObject(static_cast<ShapeType>(a_AvailableRenderObjects[i]));
	}
}

void RenderFactory::CreateRenderObject(ShapeType a_ShapeType)
{
	MeshData* t_Mesh;

	//temp
	std::vector<Vertex> t_Vertices;
	std::vector<uint32_t> t_Indices;

	switch (a_ShapeType)
	{
	default:
		t_Mesh = nullptr;
		break;
	case ShapeType::Triangle:
		t_Vertices = { {{0.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },
			{{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
		};

		t_Indices = { 0, 1, 2 };

		t_Mesh = new MeshData(t_Vertices, t_Indices);
		break;
	case ShapeType::Rectangle:
		t_Vertices = { {{-1.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
						{{1.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
						{{1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
						{{-1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}} };

		t_Indices = { 0, 1, 2, 2, 3, 0 };

		t_Mesh = new MeshData(t_Vertices, t_Indices);
		break;
	case ShapeType::SkyBoxRect:
		throw;
		break;
	case ShapeType::Pavillion:
		t_Mesh = ResourceLoader::LoadModel("../VulkanRenderer/Resources/Models/Pavillion.obj");
		break;
	}

	uint32_t t_VecPos = static_cast<uint32_t>(a_ShapeType);

	m_RenderObjectsData[t_VecPos] = new RenderObjectData(t_Mesh);
	m_CurrentLoadedObjectsID.push_back(t_VecPos);
}
