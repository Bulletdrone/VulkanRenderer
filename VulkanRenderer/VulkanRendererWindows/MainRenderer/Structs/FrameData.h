#pragma once
#include <Vulkan/vulkan.h>

struct FrameData 
{
	VkSemaphore ImageAvailableSemaphore;
	VkSemaphore RenderFinishedSemaphore;
	VkFence InFlightFence;
	VkFence ImageInFlight = VK_NULL_HANDLE;
};