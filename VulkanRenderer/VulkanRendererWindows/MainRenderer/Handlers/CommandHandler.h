#pragma once

#include <Vulkan/vulkan.h>

class CommandHandler
{
public:
	CommandHandler(VkDevice& rm_Device, VkCommandPool& rm_CommandPool, VkQueue& rm_GraphicsQueue);
	~CommandHandler();

	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer a_CommandBuffer);

private:
	VkDevice& rm_Device;
	VkCommandPool& rm_CommandPool;

	VkQueue& rm_GraphicsQueue;
};

