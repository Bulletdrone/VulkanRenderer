#include "MeshData.h"

#include <iostream>

MeshData::MeshData(std::vector<Vertex> a_Vertices)
{
	m_Vertices = a_Vertices;
}

MeshData::~MeshData()
{
}

void MeshData::DeleteBuffers(VkDevice& r_Device)
{
	vkDestroyBuffer(r_Device, mvk_VertBuffer, nullptr);
	vkFreeMemory(r_Device, mvk_VertBufferMemory, nullptr);
}