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

class CommandHandler
{
public:
	CommandHandler(VkDevice& rm_Device, VkQueue& rm_GraphicsQueue, size_t a_BufferAmount);
	~CommandHandler();

	void CreateCommandPools(QueueFamilyIndices a_QueueFamilyIndices);
	void FreeCommandPool();

	void ClearPreviousCommand(size_t a_Frame);
	VkCommandBuffer& CreateAndBeginCommand(size_t a_Frame, uint32_t a_QueueFamilyIndex, VkRenderPass& r_RenderPass, VkFramebuffer& r_SwapChainFrameBuffer, VkExtent2D& r_SwapChainExtent);
	void EndCommand(VkCommandBuffer& r_CommandBuffer);


	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer a_CommandBuffer);

	VkCommandPoolCreateInfo CreateCommandPoolInfo(uint32_t a_QueueFamilyIndex, VkCommandPoolCreateFlags a_Flags);
	VkCommandBufferAllocateInfo CreateCommandBufferInfo(VkCommandPool& r_Pool, uint32_t a_Count, VkCommandBufferLevel a_Level);

private:
	VkCommandPool mvk_CommandPool;
	std::vector<VkCommandBuffer> mvk_MainBuffers;

	VkDevice& rm_Device;

	VkQueue& rm_GraphicsQueue;
};

