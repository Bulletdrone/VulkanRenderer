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

		m_ShapeHandles[static_cast<size_t>(GeometryType::Triangle)] = a_Renderer->GenerateMesh(t_Vertices, t_Indices);
	}

	{
		//Generate QuadMesh
		std::vector<Vertex> t_Vertices =
		{ {{-1.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
			{{1.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
			{{1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
			{{-1.0f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}} };

		std::vector<uint32_t> t_Indices = { 0, 1, 2, 2, 3, 0 };

		m_ShapeHandles[static_cast<size_t>(GeometryType::Quad)] = a_Renderer->GenerateMesh(t_Vertices, t_Indices);
	}

	{
		//LoadDebugMaterial
		size_t t_VertSize = 0;
		const unsigned char* t_VertData = LoadShader("../VulkanRenderer/Resources/Shaders/debugVert.spv", t_VertSize);

		size_t t_FragSize = 0;
		const unsigned char* t_FragData = LoadShader("../VulkanRenderer/Resources/Shaders/debugFrag.spv", t_FragSize);

		ShaderHandle t_DebugVert = a_Renderer->CreateShader(t_VertData, t_VertSize);
		ShaderHandle t_DebugFrag = a_Renderer->CreateShader(t_FragData, t_FragSize);

		m_DebugMaterialHandle = a_Renderer->CreateMaterial(t_DebugVert, t_DebugFrag, 0, nullptr, 0, nullptr);
	}
}

MeshHandle GeometryFactory::GetShape(GeometryType a_Type)
{
	return m_ShapeHandles[static_cast<size_t>(a_Type)];
}

#include <fstream>
#include <iostream>
#include <assert.h>

const unsigned char* GeometryFactory::LoadShader(const char* a_FilePath, size_t& a_ShaderSize)
{
	std::ifstream t_File(a_FilePath, std::ios::ate | std::ios::binary);

	if (!t_File.is_open())
	{
		assert(false, "failed to open file!");
	}

	size_t t_FileSize = static_cast<size_t>(t_File.tellg());
	a_ShaderSize = t_FileSize;
	char* t_ReadFile = new char[t_FileSize];

	t_File.seekg(0);
	t_File.read(t_ReadFile, t_FileSize);
	t_File.close();

	return reinterpret_cast<const unsigned char*>(t_ReadFile);
}
