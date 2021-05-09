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

	void CreateTextureImage(VkImage& r_Image, VkDeviceMemory& r_ImageMemory,
		const char* a_FilePath, VkDeviceSize& r_ImageSize, int& r_TexWidth, int& r_TexHeight, int& r_TexChannels);

private:
	void CreateImage(VkImage& r_Image, VkDeviceMemory& r_ImageMemory,
		uint32_t a_Width, uint32_t a_Height, VkFormat a_Format, VkImageTiling a_Tiling, VkImageUsageFlags a_Usage, VkMemoryPropertyFlags a_Properties);
	void TransitionImageLayout(VkImage a_Image, VkFormat a_Format, VkImageLayout a_OldLayout, VkImageLayout a_NewLayout);

	//Standard Buffer Creation
	void CreateBuffer(VkDeviceSize a_Size, VkBufferUsageFlags a_Usage,
		VkMemoryPropertyFlags a_Properties, VkBuffer& r_Buffer, VkDeviceMemory& r_BufferMemory);
	void CopyBuffer(VkDeviceSize a_Size, VkBuffer& r_SrcBuffer, VkBuffer& r_DstBuffer);

	uint32_t FindMemoryType(uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties);

	VkDevice& rm_Device;
	VkPhysicalDevice& rm_PhysDevice;

	CommandHandler* p_CommandHandler;
};

