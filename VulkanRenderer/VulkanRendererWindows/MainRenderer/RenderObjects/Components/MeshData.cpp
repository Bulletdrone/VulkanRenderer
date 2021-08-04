#include "MeshData.h"

#include <iostream>

MeshData::MeshData(const std::vector<Vertex> a_Vertices, const std::vector<uint32_t> a_Indices)
{
	m_Vertices = new BufferData<Vertex>(a_Vertices);
	m_Indices = new BufferData<uint32_t>(a_Indices);
}

MeshData::~MeshData()
{
	delete m_Vertices;
	delete m_Indices;
}

void MeshData::DeleteBuffers(VkDevice& r_Device)
{
	m_Vertices->DeleteBuffers(r_Device);
	m_Indices->DeleteBuffers(r_Device);
}