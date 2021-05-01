#include "MeshData.h"

#include <iostream>

MeshData::MeshData(std::vector<Vertex> a_Vertices, VkDevice& r_Device, VkPhysicalDevice& r_PhysicalDevice)
	:	rvk_Device(r_Device), rvk_PhysicalDevice(r_PhysicalDevice)
{
	m_Vertices = a_Vertices;
	//indices = a_Indices;

	//SETUP THE THING.
	SetupMesh();
}

MeshData::~MeshData()
{
	vkDestroyBuffer(rvk_Device, mvk_VertBuffer, nullptr);
	vkFreeMemory(rvk_Device, mvk_VertBufferMemory, nullptr);
}

void MeshData::SetupMesh()
{
	VkBufferCreateInfo t_BufferInfo{};
	t_BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	t_BufferInfo.size = sizeof(m_Vertices[0]) * m_Vertices.size();	 
	t_BufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	t_BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(rvk_Device, &t_BufferInfo, nullptr, &mvk_VertBuffer) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create vertex buffer!");
	}
	
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(rvk_Device, mvk_VertBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(rvk_Device, &allocInfo, nullptr, &mvk_VertBufferMemory) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(rvk_Device, mvk_VertBuffer, mvk_VertBufferMemory, 0);

	void* data;
	vkMapMemory(rvk_Device, mvk_VertBufferMemory, 0, t_BufferInfo.size, 0, &data);
	memcpy(data, m_Vertices.data(), (size_t)t_BufferInfo.size);
	vkUnmapMemory(rvk_Device, mvk_VertBufferMemory);
}

uint32_t MeshData::FindMemoryType(uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(rvk_PhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
	{
		if ((a_TypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & a_Properties) == a_Properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}