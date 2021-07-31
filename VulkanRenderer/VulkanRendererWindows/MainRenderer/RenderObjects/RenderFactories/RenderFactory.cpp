#include "RenderFactory.h"

RenderFactory::RenderFactory()
{
	std::vector<ShapeType> t_Shape;
	t_Shape.push_back(ShapeType::Triangle);
	t_Shape.push_back(ShapeType::Rectangle);


	SetSceneObjects(t_Shape);
}

RenderFactory::~RenderFactory()
{
	ClearSceneObjects();
}

BaseRenderObject* RenderFactory::CreateRenderObject(const size_t a_RenderID, ShapeType a_ShapeType, Transform* a_Transform, uint32_t a_PipelineID)
{
	return new RenderShape(a_RenderID, a_Transform, a_PipelineID, m_RenderObjectsData[static_cast<size_t>(a_ShapeType)]);
}

//Clear all the current loaded objects.
void RenderFactory::ClearSceneObjects()
{
	for (size_t i = 0; i < m_CurrentLoadedObjectsID.size(); i++)
	{
		delete m_RenderObjectsData[m_CurrentLoadedObjectsID[i]];
	}
	m_RenderObjectsData.clear();
	m_CurrentLoadedObjectsID.clear();
}

void RenderFactory::SetSceneObjects(std::vector<ShapeType>& a_AvailableShapes)
{
	m_RenderObjectsData.resize(m_AllShapes);

	for (size_t i = 0; i < a_AvailableShapes.size(); i++)
	{
		size_t t_VecPos = static_cast<size_t>(a_AvailableShapes[i]);

		//Remember the Position of the Vector for later removal.
		m_CurrentLoadedObjectsID.push_back(t_VecPos);

		MeshData* t_Mesh;

		//temp
		std::vector<Vertex> t_Vertices;
		std::vector<uint16_t> t_Indices;

		switch (a_AvailableShapes[i])
		{
		case ShapeType::Triangle:
			t_Vertices = { {{0.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },
				{{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
				{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
		};

			t_Indices = { 0, 1, 2 };

			t_Mesh = new MeshData(t_Vertices, t_Indices);

			m_RenderObjectsData[t_VecPos] = new RenderObjectData(t_Mesh);
			break;
		case ShapeType::Rectangle:
			t_Vertices = { {{-1.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
							{{1.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
							{{1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
							{{-1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}} };

			t_Indices = { 0, 1, 2, 2, 3, 0 };

			t_Mesh = new MeshData(t_Vertices, t_Indices);

			m_RenderObjectsData[t_VecPos] = new RenderObjectData(t_Mesh);
			break;
		case ShapeType::SkyBoxRect:
			throw;
			break;
		}
	}
}

void RenderFactory::ResetSceneObjects(std::vector<ShapeType>& a_AvailableShapes)
{
	ClearSceneObjects();
	SetSceneObjects(a_AvailableShapes);
}