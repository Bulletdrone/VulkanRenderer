#include "CommandHandler.h"

CommandHandler::CommandHandler(VkDevice& r_Device, VkCommandPool& r_CommandPool, VkQueue& r_GraphicsQueue)
    :  rm_Device(r_Device), rm_CommandPool(r_CommandPool), rm_GraphicsQueue(r_GraphicsQueue)
{}

CommandHandler::~CommandHandler()
{}

VkCommandBuffer CommandHandler::BeginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo t_AllocInfo{};
    t_AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    t_AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    t_AllocInfo.commandPool = rm_CommandPool;
    t_AllocInfo.commandBufferCount = 1;

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

    vkFreeCommandBuffers(rm_Device, rm_CommandPool, 1, &a_CommandBuffer);
}