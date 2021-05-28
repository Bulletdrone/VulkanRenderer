#include "CommandHandler.h"

#include <fstream>

CommandHandler::CommandHandler(VkDevice& r_Device, VkQueue& r_GraphicsQueue)
    :  rm_Device(r_Device), rm_GraphicsQueue(r_GraphicsQueue)
{}

CommandHandler::~CommandHandler()
{
    vkDestroyCommandPool(rm_Device, mvk_CommandPool, nullptr);
}

void CommandHandler::CreateCommandPools(QueueFamilyIndices a_QueueFamilyIndices)
{
    VkCommandPoolCreateInfo t_PoolInfo = CreateCommandPoolInfo(a_QueueFamilyIndices.graphicsFamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    if (vkCreateCommandPool(rm_Device, &t_PoolInfo, nullptr, &mvk_CommandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void CommandHandler::FreeCommandPool()
{
    //vkFreeCommandBuffers(rm_Device, mvk_CommandPool, static_cast<uint32_t>(mvk_CommandBuffers.size()), mvk_CommandBuffers.data());
    vkFreeCommandBuffers(rm_Device, mvk_CommandPool, 1, mvk_MainBuffer);

}

void CommandHandler::CreateCommand()
{
    //allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo t_CmdAllocInfo = CreateCommandBufferInfo(mvk_CommandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    if (vkAllocateCommandBuffers(rm_Device, &t_CmdAllocInfo, &mvk_MainBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

VkCommandBuffer CommandHandler::BeginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo t_AllocInfo = CreateCommandBufferInfo(mvk_CommandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    VkCommandBuffer t_CommandBuffer;
    vkAllocateCommandBuffers(rm_Device, &t_AllocInfo, &t_CommandBuffer);

    VkCommandBufferBeginInfo t_BeginInfo{};
    t_BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    t_BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(t_CommandBuffer, &t_BeginInfo);

    return t_CommandBuffer;
}

void CommandHandler::EndSingleTimeCommands(VkCommandBuffer a_CommandBuffer)
{
    vkEndCommandBuffer(a_CommandBuffer);

    VkSubmitInfo t_SubmitInfo{};
    t_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    t_SubmitInfo.commandBufferCount = 1;
    t_SubmitInfo.pCommandBuffers = &a_CommandBuffer;

    vkQueueSubmit(rm_GraphicsQueue, 1, &t_SubmitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(rm_GraphicsQueue);

    vkFreeCommandBuffers(rm_Device, mvk_CommandPool, 1, &a_CommandBuffer);
}

VkCommandPoolCreateInfo CommandHandler::CreateCommandPoolInfo(uint32_t a_QueueFamilyIndex, VkCommandPoolCreateFlags a_Flags)
{
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;

    info.queueFamilyIndex = a_QueueFamilyIndex;
    info.flags = a_Flags;
    return info;
}

VkCommandBufferAllocateInfo CommandHandler::CreateCommandBufferInfo(VkCommandPool& r_Pool, uint32_t a_Count, VkCommandBufferLevel a_Level)
{
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;

    info.commandPool = r_Pool;
    info.commandBufferCount = a_Count;
    info.level = a_Level;
    return info;
}