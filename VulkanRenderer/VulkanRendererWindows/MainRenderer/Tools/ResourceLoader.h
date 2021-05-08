#pragma once
#include <fstream>
#include <vector>

#include <Vulkan/vulkan.h>

class ResourceLoader
{
public:
	static std::vector<char> ReadFile(const std::string& filename);

	static void CreateBuffer(VkDevice& r_Device, VkPhysicalDevice& r_PhysDevice, VkDeviceSize a_Size, VkBufferUsageFlags a_Usage,
		VkMemoryPropertyFlags a_Properties, VkBuffer& r_Buffer, VkDeviceMemory& r_BufferMemory);

	static void CreateImage(VkDevice& r_Device, VkPhysicalDevice& r_PhysDevice, 
		const char* a_FilePath, VkDeviceSize& r_ImageSize, int* p_TexWidth, int* p_TexHeight, int* p_TexChannels);

private:
	static uint32_t FindMemoryType(uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties, VkPhysicalDevice& r_PhysDevice);
};