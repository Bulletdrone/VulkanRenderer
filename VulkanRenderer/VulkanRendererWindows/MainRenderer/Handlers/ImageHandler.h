#pragma once

#include <Vulkan/vulkan.h>

class ImageHandler
{
public:
	ImageHandler(VkDevice& r_Device, float a_AnisotropicMax);
	~ImageHandler();

	VkImageView CreateImageView(VkImage a_Image, VkFormat a_Format);
	VkSampler CreateTextureSampler();

private:
	VkDevice& rm_Device;

	float m_AnisotropicMax;

};

