#include "BufferHandler.h"

#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "CommandHandler.h"

BufferHandler::BufferHandler(VkDevice& r_Device, VkPhysicalDevice& r_PhysDevice, CommandHandler* a_CommandHandler)
    :   rm_Device(r_Device), rm_PhysDevice(r_PhysDevice)
{
    p_CommandHandler = a_CommandHandler;
}

BufferHandler::~BufferHandler()
{}

#pragma region Specific Buffer Creation

void BufferHandler::CreateVertexBuffers(BufferData<Vertex>* a_VertexData)
{
    VkDeviceSize t_BufferSize = a_VertexData->CreateBufferSize();

    VkBuffer t_StagingBuffer;
    VkDeviceMemory t_StagingBufferMemory;
    CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        t_StagingBuffer, t_StagingBufferMemory);

    void* t_Data;
    vkMapMemory(rm_Device, t_StagingBufferMemory, 0, t_BufferSize, 0, &t_Data);
    memcpy(t_Data, a_VertexData->GetElements().data(), (size_t)t_BufferSize);
    vkUnmapMemory(rm_Device, t_StagingBufferMemory);

    CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        a_VertexData->GetBuffer(), a_VertexData->GetBufferMemory());

    CopyBuffer(t_BufferSize, t_StagingBuffer, a_VertexData->GetBuffer());

    vkDestroyBuffer(rm_Device, t_StagingBuffer, nullptr);
    vkFreeMemory(rm_Device, t_StagingBufferMemory, nullptr);
}

void BufferHandler::CreateIndexBuffers(BufferData<uint16_t>* a_IndexData)
{
    VkDeviceSize t_BufferSize = a_IndexData->CreateBufferSize();

    VkBuffer t_StagingBuffer;
    VkDeviceMemory t_StagingBufferMemory;
    CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        t_StagingBuffer, t_StagingBufferMemory);

    void* t_Data;
    vkMapMemory(rm_Device, t_StagingBufferMemory, 0, t_BufferSize, 0, &t_Data);
    memcpy(t_Data, a_IndexData->GetElements().data(), (size_t)t_BufferSize);
    vkUnmapMemory(rm_Device, t_StagingBufferMemory);

    CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        a_IndexData->GetBuffer(), a_IndexData->GetBufferMemory());

    CopyBuffer(t_BufferSize, t_StagingBuffer, a_IndexData->GetBuffer());

    vkDestroyBuffer(rm_Device, t_StagingBuffer, nullptr);
    vkFreeMemory(rm_Device, t_StagingBufferMemory, nullptr);
}

void BufferHandler::CreateUniformBuffers(std::vector<VkBuffer>& r_UniformBuffers, 
    std::vector<VkDeviceMemory>& r_UniformBuffersMemory, const size_t a_SwampChainSize)
{
    VkDeviceSize t_BufferSize = sizeof(ViewProjection);

    r_UniformBuffers.resize(a_SwampChainSize);
    r_UniformBuffersMemory.resize(a_SwampChainSize);

    for (size_t i = 0; i < a_SwampChainSize; i++)
    {
        CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            r_UniformBuffers[i], r_UniformBuffersMemory[i]);
    }

    VkDeviceSize t_UniBufferSize = sizeof(ViewProjection);

    r_UniformBuffers.resize(a_SwampChainSize);
    r_UniformBuffersMemory.resize(a_SwampChainSize);

    for (size_t i = 0; i < a_SwampChainSize; i++)
    {
        CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            r_UniformBuffers[i], r_UniformBuffersMemory[i]);
    }
}

#pragma endregion

#pragma region ImageCreation

void BufferHandler::CreateTextureImage(VkImage& r_Image, VkDeviceMemory& r_ImageMemory,
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

    CreateBuffer(r_ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        t_StagingBuffer, t_StagingBufferMemory);

    void* data;
    vkMapMemory(rm_Device, t_StagingBufferMemory, 0, r_ImageSize, 0, &data);
    memcpy(data, t_ImagePixels, static_cast<size_t>(r_ImageSize));
    vkUnmapMemory(rm_Device, t_StagingBufferMemory);

    stbi_image_free(t_ImagePixels);

    CreateImage(r_Image, r_ImageMemory, r_TexWidth, r_TexHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void BufferHandler::CreateImage(VkImage& r_Image, VkDeviceMemory& r_ImageMemory,
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
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, a_Properties);

    if (vkAllocateMemory(rm_Device, &allocInfo, nullptr, &r_ImageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(rm_Device, r_Image, r_ImageMemory, 0);
}

void BufferHandler::TransitionImageLayout(VkImage a_Image, VkFormat a_Format, VkImageLayout a_OldLayout, VkImageLayout a_NewLayout)
{
    VkCommandBuffer t_CommandBuffer = p_CommandHandler->BeginSingleTimeCommands();

    VkImageMemoryBarrier t_Barrier{};
    t_Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    t_Barrier.oldLayout = a_OldLayout;
    t_Barrier.newLayout = a_NewLayout;

    t_Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    t_Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    t_Barrier.image = a_Image;
    t_Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    t_Barrier.subresourceRange.baseMipLevel = 0;
    t_Barrier.subresourceRange.levelCount = 1;
    t_Barrier.subresourceRange.baseArrayLayer = 0;
    t_Barrier.subresourceRange.layerCount = 1;

    t_Barrier.srcAccessMask = 0; // TODO
    t_Barrier.dstAccessMask = 0; // TODO

    vkCmdPipelineBarrier(
        t_CommandBuffer,
        0 /* TODO */, 0 /* TODO */,
        0,
        0, nullptr,
        0, nullptr,
        1, &t_Barrier
    );

    p_CommandHandler->EndSingleTimeCommands(t_CommandBuffer);
}

#pragma endregion


//PRIVATES

#pragma region Buffer Helpers

void BufferHandler::CreateBuffer(VkDeviceSize a_Size, VkBufferUsageFlags a_Usage, VkMemoryPropertyFlags a_Properties, VkBuffer& r_Buffer, VkDeviceMemory& r_BufferMemory)
{
    VkBufferCreateInfo t_BufferInfo{};
    t_BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    t_BufferInfo.size = a_Size;
    t_BufferInfo.usage = a_Usage;
    t_BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkResult t;
    if ((t = vkCreateBuffer(rm_Device, &t_BufferInfo, nullptr, &r_Buffer)) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements t_MemRequirements;
    vkGetBufferMemoryRequirements(rm_Device, r_Buffer, &t_MemRequirements);

    VkMemoryAllocateInfo t_AllocInfo{};
    t_AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    t_AllocInfo.allocationSize = t_MemRequirements.size;
    t_AllocInfo.memoryTypeIndex = FindMemoryType(t_MemRequirements.memoryTypeBits, a_Properties);

    if (vkAllocateMemory(rm_Device, &t_AllocInfo, nullptr, &r_BufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(rm_Device, r_Buffer, r_BufferMemory, 0);
}

void BufferHandler::CopyBuffer(VkDeviceSize a_Size, VkBuffer& r_SrcBuffer, VkBuffer& r_DstBuffer)
{
    VkCommandBuffer t_CommandBuffer = p_CommandHandler->BeginSingleTimeCommands();

    VkBufferCopy t_CopyRegion{};
    t_CopyRegion.size = a_Size;
    vkCmdCopyBuffer(t_CommandBuffer, r_SrcBuffer, r_DstBuffer, 1, &t_CopyRegion);

    p_CommandHandler->EndSingleTimeCommands(t_CommandBuffer);
}

#pragma endregion


//PRIVATE FUNCTION TO FIND MEMORYTYPE
uint32_t BufferHandler::FindMemoryType(uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(rm_PhysDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((a_TypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & a_Properties) == a_Properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}