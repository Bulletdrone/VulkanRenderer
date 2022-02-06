#include "RenderFactory.h"

#include <VulkanEngine/Framework/ResourceSystem/ResourceAllocator.h>
#include <VulkanEngine/Framework/ResourceSystem/MeshResource.h>

RenderFactory::RenderFactory()
{}

RenderFactory::~RenderFactory()
{}

BaseRenderObject* RenderFactory::CreateRenderObject(const uint32_t a_RenderID, Transform* a_Transform, Material& a_Material, const char* a_MeshPath)
{
	return new RenderShape(a_RenderID, a_Transform, a_Material, 
		new RenderObjectData(&Engine::ResourceAllocator::GetInstance().GetResource<Engine::Resource::MeshResource>(a_MeshPath, Engine::Resource::ResourceType::Mesh).meshData));
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
