#pragma once

struct TextureData
{
	void UpdateDescriptor()
	{
		descriptorInfo.sampler = textureSampler;
		descriptorInfo.imageView = textureImageView;
		descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	void DeleteBuffers(VkDevice& r_Device)
	{
		vkDestroySampler(r_Device, textureSampler, nullptr);
		vkDestroyImageView(r_Device, textureImageView, nullptr);

		vkDestroyImage(r_Device, textureImage, nullptr);
		vkFreeMemory(r_Device, textureImageMemory, nullptr);
	}

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	VkDescriptorImageInfo descriptorInfo;
};