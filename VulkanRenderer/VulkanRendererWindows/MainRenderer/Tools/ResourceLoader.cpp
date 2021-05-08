#include "ResourceLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

std::vector<char> ResourceLoader::ReadFile(const std::string& filename) {
    std::ifstream t_File(filename, std::ios::ate | std::ios::binary);
        
    if (!t_File.is_open()) 
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t t_FileSize = (size_t)t_File.tellg();
    std::vector<char> t_Buffer(t_FileSize);

    t_File.seekg(0);
    t_File.read(t_Buffer.data(), t_FileSize);

    t_File.close();

    return t_Buffer;
}

void ResourceLoader::CreateBuffer(VkDevice& r_Device, VkPhysicalDevice& r_PhysDevice, VkDeviceSize a_Size, VkBufferUsageFlags a_Usage, VkMemoryPropertyFlags a_Properties, VkBuffer& r_Buffer, VkDeviceMemory& r_BufferMemory)
{
    VkBufferCreateInfo t_BufferInfo{};
    t_BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    t_BufferInfo.size = a_Size;
    t_BufferInfo.usage = a_Usage;
    t_BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(r_Device, &t_BufferInfo, nullptr, &r_Buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements t_MemRequirements;
    vkGetBufferMemoryRequirements(r_Device, r_Buffer, &t_MemRequirements);

    VkMemoryAllocateInfo t_AllocInfo{};
    t_AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    t_AllocInfo.allocationSize = t_MemRequirements.size;
    t_AllocInfo.memoryTypeIndex = FindMemoryType(t_MemRequirements.memoryTypeBits, a_Properties, r_PhysDevice);

    if (vkAllocateMemory(r_Device, &t_AllocInfo, nullptr, &r_BufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(r_Device, r_Buffer, r_BufferMemory, 0);
}

void ResourceLoader::CreateImage(VkDevice& r_Device, VkPhysicalDevice& r_PhysDevice, 
    const char* a_FilePath, VkDeviceSize& r_ImageSize, int* p_TexWidth, int* p_TexHeight, int* p_TexChannels)
{
    stbi_uc* t_ImagePixels = stbi_load(a_FilePath, p_TexHeight, p_TexHeight, p_TexChannels, STBI_rgb_alpha);
    
    r_ImageSize = static_cast<VkDeviceSize>(*p_TexWidth * *p_TexHeight * 4);

    if (!t_ImagePixels) 
    {
        throw std::runtime_error("failed to load texture image!");
    }

    VkBuffer t_StagingBuffer;
    VkDeviceMemory t_StagingBufferMemory;

    CreateBuffer(r_Device, r_PhysDevice, r_ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        t_StagingBuffer, t_StagingBufferMemory);

    void* data;
    vkMapMemory(r_Device, t_StagingBufferMemory, 0, r_ImageSize, 0, &data);
    memcpy(r_Device, t_ImagePixels, static_cast<size_t>(r_ImageSize));
    vkUnmapMemory(r_Device, t_StagingBufferMemory);
}

//PRIVATE FUNCTION TO FIND MEMORYTYPE
uint32_t ResourceLoader::FindMemoryType(uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties, VkPhysicalDevice& r_PhysDevice)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(r_PhysDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((a_TypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & a_Properties) == a_Properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}