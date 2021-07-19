#include "BufferHandler.h"

#include <fstream>

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