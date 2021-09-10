#pragma once
#include <Vulkan/vulkan.h>

namespace VkInit
{
	inline VkDescriptorBufferInfo CreateDescriptorBufferInfo(VkBuffer& r_UniformBuffer, VkDeviceSize a_Offset, VkDeviceSize a_BufferSize)
	{
		VkDescriptorBufferInfo t_BufferInfo{};
		t_BufferInfo.buffer = r_UniformBuffer;
		t_BufferInfo.offset = 0;
		t_BufferInfo.range = a_BufferSize;

		return t_BufferInfo;
	}

	inline VkDescriptorImageInfo CreateDescriptorImageInfo(VkImageLayout a_ImageLayout, VkImageView& r_ImageView, VkSampler& r_Sampler)
	{
		VkDescriptorImageInfo t_ImageInfo{};
		t_ImageInfo.imageLayout = a_ImageLayout;
		t_ImageInfo.imageView = r_ImageView;
		t_ImageInfo.sampler = r_Sampler;

		return t_ImageInfo;
	}
}