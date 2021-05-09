#include "ImageHandler.h"

#include "BufferHandler.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <fstream>

ImageHandler::ImageHandler(VkDevice& r_Device, float a_AnisotropicMax, BufferHandler* a_BufferHandler)
	:	rm_Device(r_Device)
{
	m_AnisotropicMax = a_AnisotropicMax;
    p_BufferHandler = a_BufferHandler;
}

ImageHandler::~ImageHandler()
{}

void ImageHandler::CreateImage(VkImage& r_Image, VkDeviceMemory& r_ImageMemory,
    uint32_t a_Width, uint32_t a_Height, VkFormat a_Format, VkImageTiling a_Tiling, VkImageUsageFlags a_Usage, VkMemoryPropertyFlags a_Properties)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(a_Width);
    imageInfo.extent.height = static_cast<uint32_t>(a_Height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = a_Format;
    imageInfo.tiling = a_Tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = a_Usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;

    if (vkCreateImage(rm_Device, &imageInfo, nullptr, &r_Image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(rm_Device, r_Image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = p_BufferHandler->FindMemoryType(memRequirements.memoryTypeBits, a_Properties);

    if (vkAllocateMemory(rm_Device, &allocInfo, nullptr, &r_ImageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(rm_Device, r_Image, r_ImageMemory, 0);
}

void ImageHandler::CreateTextureImage(VkImage& r_Image, VkDeviceMemory& r_ImageMemory,
    const char* a_FilePath, VkDeviceSize& r_ImageSize, int& r_TexWidth, int& r_TexHeight, int& r_TexChannels)
{
    stbi_uc* t_ImagePixels = stbi_load(a_FilePath, &r_TexWidth, &r_TexHeight, &r_TexChannels, STBI_rgb_alpha);

    r_ImageSize = static_cast<VkDeviceSize>(r_TexWidth * r_TexHeight * 4);

    if (!t_ImagePixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    VkBuffer t_StagingBuffer;
    VkDeviceMemory t_StagingBufferMemory;

    p_BufferHandler->CreateBuffer(r_ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        t_StagingBuffer, t_StagingBufferMemory);

    void* data;
    vkMapMemory(rm_Device, t_StagingBufferMemory, 0, r_ImageSize, 0, &data);
    memcpy(data, t_ImagePixels, static_cast<size_t>(r_ImageSize));
    vkUnmapMemory(rm_Device, t_StagingBufferMemory);

    stbi_image_free(t_ImagePixels);

    CreateImage(r_Image, r_ImageMemory, r_TexWidth, r_TexHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    p_BufferHandler->TransitionImageLayout(r_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    p_BufferHandler->CopyBufferToImage(t_StagingBuffer, r_Image, static_cast<uint32_t>(r_TexWidth), static_cast<uint32_t>(r_TexHeight));

    p_BufferHandler->TransitionImageLayout(r_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(rm_Device, t_StagingBuffer, nullptr);
    vkFreeMemory(rm_Device, t_StagingBufferMemory, nullptr);
}

VkImageView ImageHandler::CreateImageView(VkImage a_Image, VkFormat a_Format, VkImageAspectFlags a_AspectFlags)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = a_Image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = a_Format;
	viewInfo.subresourceRange.aspectMask = a_AspectFlags;
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
