#pragma once

#include <Vulkan/vulkan.h>

#include <optional>
#include <vector>

#include <RenderObjects/BaseRenderObject.h>

//Special struct for the Family Queue.
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct DrawCommands
{
	VkCommandBuffer mvk_MainBuffer;
	std::vector<VkCommandBuffer> CommandBuffers;
};

class CommandHandler
{
public:
	CommandHandler(VkDevice& rm_Device, VkQueue& rm_GraphicsQueue);
	~CommandHandler();

	void CreateCommandPools(QueueFamilyIndices a_QueueFamilyIndices);
	void FreeCommandPool();
	void FreeDynamicCommandBuffers(uint32_t a_Frame);

	void ClearPreviousCommand(size_t a_Frame);
	VkCommandBuffer& CreateAndBeginCommand(size_t a_Frame, uint32_t a_QueueFamilyIndex, VkRenderPass& r_RenderPass, VkFramebuffer& r_SwapChainFrameBuffer, VkExtent2D& r_SwapChainExtent);
	void EndCommand(VkCommandBuffer& r_CommandBuffer);


	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer a_CommandBuffer);

	VkCommandPoolCreateInfo CreateCommandPoolInfo(uint32_t a_QueueFamilyIndex, VkCommandPoolCreateFlags a_Flags);
	VkCommandBufferAllocateInfo CreateCommandBufferInfo(VkCommandPool& r_Pool, uint32_t a_Count, VkCommandBufferLevel a_Level);

	DrawCommands& GetDrawCommands(size_t a_Frame) { return m_DrawCommands[a_Frame]; }

private:
	VkCommandPool mvk_CommandPool;

	//Buffer Commands
	VkCommandBuffer mvk_MainBuffers[2];
	DrawCommands m_DrawCommands[2];
	//std::vector<VkCommandBuffer> mvk_CommandBuffers;

	VkDevice& rm_Device;

	VkQueue& rm_GraphicsQueue;
};

