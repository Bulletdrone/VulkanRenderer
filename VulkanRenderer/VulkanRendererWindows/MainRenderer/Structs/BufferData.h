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

template <typename T>
class BufferData
{
public:
	BufferData(const std::vector<T> a_Elements);
	~BufferData();

	void DeleteBuffers(VkDevice& r_Device);

	const VkDeviceSize CreateBufferSize() const;

	const std::vector<T>& GetElements() const { return elements; }
	const size_t GetElementCount() const { return elements.size(); }

	VkBuffer& GetBuffer() { return vkBuffer; }
	VkDeviceMemory& GetBufferMemory() { return vkBufferMemory; }


private:
	const std::vector<T> elements;

	VkBuffer vkBuffer;
	VkDeviceMemory vkBufferMemory;
};


template<typename T>
inline BufferData<T>::BufferData(const std::vector<T> a_Elements)
	:	elements(a_Elements)
{}


template<typename T>
inline BufferData<T>::~BufferData()
{}

template<typename T>
inline void BufferData<T>::DeleteBuffers(VkDevice & r_Device)
{
	vkDestroyBuffer(r_Device, vkBuffer, nullptr);
	vkFreeMemory(r_Device, vkBufferMemory, nullptr);
}


template <typename T>
inline const VkDeviceSize BufferData<T>::CreateBufferSize() const //Returns the vertexBuffer in bytes.
{
	return sizeof(elements[0]) * elements.size();
}