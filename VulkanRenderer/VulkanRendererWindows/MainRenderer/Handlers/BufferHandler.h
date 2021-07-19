#pragma once
#include <vector>

#include <Vulkan/vulkan.h>
#include <Structs/BufferData.h>

class CommandHandler;

class BufferHandler
{
public:
	BufferHandler(VkDevice& r_Device, VkPhysicalDevice& r_PhysDevice, CommandHandler* p_CommandHandler);
	~BufferHandler();

	void CreateVertexBuffers(BufferData<Vertex>* a_VertexData);
	void CreateIndexBuffers(BufferData<uint16_t>* a_IndexData);
	void CreateUniformBuffers(std::vector<VkBuffer>& r_UniformBuffers,
		std::vector<VkDeviceMemory>& r_UniformBuffersMemory, const size_t a_SwampChainSize);

	//Standard Buffer Creation
	void CreateBuffer(VkDeviceSize a_Size, VkBufferUsageFlags a_Usage,
		VkMemoryPropertyFlags a_Properties, VkBuffer& r_Buffer, VkDeviceMemory& r_BufferMemory);
	void CopyBuffer(VkDeviceSize a_Size, VkBuffer& r_SrcBuffer, VkBuffer& r_DstBuffer);

private:
	VkDevice& rm_Device;
	VkPhysicalDevice& rm_PhysDevice;

	CommandHandler* p_CommandHandler;
};