#pragma once

#include <Vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <array>
#include <vector>

struct Vertex
{
	glm::vec3 position;

	glm::vec3 color;

	static VkVertexInputBindingDescription GetBindingDescription() 
	{
		VkVertexInputBindingDescription t_BindingDescription{};
		t_BindingDescription.binding = 0;
		t_BindingDescription.stride = sizeof(Vertex);
		t_BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return t_BindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() 
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

class MeshData
{
public:
	MeshData(std::vector<Vertex> a_Vertices, VkDevice& r_Device, VkPhysicalDevice& r_PhysicalDevice);
	~MeshData();

	void SetupMesh();
	uint32_t FindMemoryType(uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties);

	//Getters
	const size_t GetVertexCount() const { return m_Vertices.size(); }

	const VkBuffer GetVertBuffer() const { return mvk_VertBuffer; }
	const VkDeviceMemory GetVertBufferMemory() const { return mvk_VertBufferMemory; }

private:
	std::vector<Vertex> m_Vertices;

	VkBuffer mvk_VertBuffer;
	VkDeviceMemory mvk_VertBufferMemory;

	VkDevice& rvk_Device;
	VkPhysicalDevice& rvk_PhysicalDevice;
};