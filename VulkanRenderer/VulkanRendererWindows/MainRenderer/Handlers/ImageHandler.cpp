#include "ImageHandler.h"

#include <fstream>

ImageHandler::ImageHandler(VkDevice& r_Device, float a_AnisotropicMax)
	:	rm_Device(r_Device)
{
	m_AnisotropicMax = a_AnisotropicMax;
}

ImageHandler::~ImageHandler()
{}

VkImageView ImageHandler::CreateImageView(VkImage a_Image, VkFormat a_Format)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = a_Image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = a_Format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView t_ImageView;
	if (vkCreateImageView(rm_Device, &viewInfo, nullptr, &t_ImageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return t_ImageView;
}

VkSampler ImageHandler::CreateTextureSampler()
{
	VkSamplerCreateInfo t_SamplerInfo{};
	t_SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	t_SamplerInfo.magFilter = VK_FILTER_LINEAR;
	t_SamplerInfo.minFilter = VK_FILTER_LINEAR;

	t_SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	t_SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	t_SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	t_SamplerInfo.anisotropyEnable = VK_TRUE;
	t_SamplerInfo.maxAnisotropy = m_AnisotropicMax;

	t_SamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	t_SamplerInfo.unnormalizedCoordinates = VK_FALSE;

	t_SamplerInfo.compareEnable = VK_FALSE;
	t_SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	t_SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	t_SamplerInfo.mipLodBias = 0.0f;
	t_SamplerInfo.minLod = 0.0f;
	t_SamplerInfo.maxLod = 0.0f;

	VkSampler t_Sampler;
	if (vkCreateSampler(rm_Device, &t_SamplerInfo, nullptr, &t_Sampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler!");
	}
	return t_Sampler;
}
