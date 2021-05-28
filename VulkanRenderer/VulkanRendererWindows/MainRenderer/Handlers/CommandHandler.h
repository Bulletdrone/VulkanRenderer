#pragma once

#include <Vulkan/vulkan.h>

#include <optional>
#include <vector>

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
	~DrawCommands();

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

	void CreateCommand();

	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer a_CommandBuffer);

	VkCommandPoolCreateInfo CreateCommandPoolInfo(uint32_t a_QueueFamilyIndex, VkCommandPoolCreateFlags a_Flags);
	VkCommandBufferAllocateInfo CreateCommandBufferInfo(VkCommandPool& r_Pool, uint32_t a_Count, VkCommandBufferLevel a_Level);

	DrawCommands& GetDrawCommands() { return m_DrawCommands; }

private:
	VkCommandPool mvk_CommandPool;

	//Buffer Commands

	DrawCommands m_DrawCommands;
	//std::vector<VkCommandBuffer> mvk_CommandBuffers;

	VkDevice& rm_Device;

	VkQueue& rm_GraphicsQueue;
};

