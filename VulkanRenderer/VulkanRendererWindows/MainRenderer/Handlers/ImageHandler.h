#pragma once

#include <Vulkan/vulkan.h>

class BufferHandler;

class ImageHandler
{
public:
	ImageHandler(VkDevice& r_Device, float a_AnisotropicMax, BufferHandler* a_BufferHandler);
	~ImageHandler();

	void CreateImage(VkImage& r_Image, VkDeviceMemory& r_ImageMemory,
		uint32_t a_Width, uint32_t a_Height, VkFormat a_Format, VkImageTiling a_Tiling, VkImageUsageFlags a_Usage, VkMemoryPropertyFlags a_Properties);
	void CreateTextureImage(VkImage& r_Image, VkDeviceMemory& r_ImageMemory,
		const char* a_FilePath, VkDeviceSize& r_ImageSize, int& r_TexWidth, int& r_TexHeight, int& r_TexChannels);


	VkImageView CreateImageView(VkImage a_Image, VkFormat a_Format, VkImageAspectFlags a_AspectFlags);
	VkSampler CreateTextureSampler();

private:

	VkDevice& rm_Device;

	float m_AnisotropicMax;

	BufferHandler* p_BufferHandler;
};

