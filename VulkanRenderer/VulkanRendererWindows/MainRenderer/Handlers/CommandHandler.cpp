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
    //vkFreeCommandBuffers(rm_Device, mvk_CommandPool, 1, &m_DrawCommands.mvk_MainBuffer);
}

void CommandHandler::FreeDynamicCommandBuffers(uint32_t a_Frame)
{
    if (m_DrawCommands[a_Frame].CommandBuffers.size() > 0)
    {
        vkFreeCommandBuffers(rm_Device, mvk_CommandPool, static_cast<uint32_t>(m_DrawCommands[a_Frame].CommandBuffers.size()), m_DrawCommands[a_Frame].CommandBuffers.data());
        m_DrawCommands[a_Frame].CommandBuffers.clear();
    }
}

void CommandHandler::CreateCommand(size_t a_Frame, uint32_t a_QueueFamilyIndex, BaseRenderObject* a_RenderObject, 
    VkRenderPass& r_RenderPass, VkFramebuffer& r_SwapChainFrameBuffer, VkExtent2D& r_SwapChainExtent)
{
    VkCommandBuffer t_CommandBuffer;
    VkCommandPoolCreateInfo t_CommandPoolInfo = CreateCommandPoolInfo(a_QueueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    //allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo t_CmdAllocInfo = CreateCommandBufferInfo(mvk_CommandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    if (vkAllocateCommandBuffers(rm_Device, &t_CmdAllocInfo, &t_CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }

    VkCommandBufferBeginInfo t_BeginInfo{};
    t_BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    t_BeginInfo.flags = 0; // Optional
    t_BeginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(t_CommandBuffer, &t_BeginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo t_RenderPassInfo{};
    t_RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    t_RenderPassInfo.renderPass = r_RenderPass;
    t_RenderPassInfo.framebuffer = r_SwapChainFrameBuffer;

    t_RenderPassInfo.renderArea.offset = { 0, 0 };
    t_RenderPassInfo.renderArea.extent = r_SwapChainExtent;

    std::array<VkClearValue, 2> t_ClearValues{};
    t_ClearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    t_ClearValues[1].depthStencil = { 1.0f, 0 };

    t_RenderPassInfo.clearValueCount = static_cast<uint32_t>(t_ClearValues.size());;
    t_RenderPassInfo.pClearValues = t_ClearValues.data();


    //Begin the drawing.
    vkCmdBeginRenderPass(t_CommandBuffer, &t_RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(t_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, a_RenderObject->GetPipeLineData()->pipeLine);

    //BIND THE UNIFORM BUFFER.
    vkCmdBindDescriptorSets(t_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, a_RenderObject->GetPipeLineData()->pipeLineLayout, 0, 1, &a_RenderObject->GetPipeLineData()->p_DescriptorData->descriptorSets[0], 0, nullptr);


    //Setting up object.
    VkBuffer t_VertexBuffers[] = { a_RenderObject->GetVertexData()->GetBuffer() };
    VkDeviceSize t_Offsets[] = { 0 };
    vkCmdBindVertexBuffers(t_CommandBuffer, 0, 1, t_VertexBuffers, t_Offsets);

    vkCmdBindIndexBuffer(t_CommandBuffer, a_RenderObject->GetIndexData()->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);

    //ConstantPushing, look in ShaderManager for more information.
    vkCmdPushConstants(t_CommandBuffer, a_RenderObject->GetPipeLineData()->pipeLineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(InstanceModel), &a_RenderObject->GetModelMatrix());
    
    //vkCmdDraw(mvk_CommandBuffers[i], static_cast<uint32_t>(TEMPMESH->GetVertexData()->GetElementCount()), 1, 0, 0);
    vkCmdDrawIndexed(t_CommandBuffer, static_cast<uint32_t>(a_RenderObject->GetIndexData()->GetElementCount()), 1, 0, 0, 0);

    //Stop recording the buffer.
    vkCmdEndRenderPass(t_CommandBuffer);

    if (vkEndCommandBuffer(t_CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    m_DrawCommands[a_Frame].CommandBuffers.push_back(t_CommandBuffer);
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