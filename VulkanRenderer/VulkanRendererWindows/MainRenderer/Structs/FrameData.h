#pragma once
#pragma warning (push, 0)
#include <Vulkan/vulkan.h>
#pragma warning (pop)

struct FrameData 
{
	VkSemaphore ImageAvailableSemaphore = VK_NULL_HANDLE;
	VkSemaphore RenderFinishedSemaphore = VK_NULL_HANDLE;
	VkFence InFlightFence = VK_NULL_HANDLE;
	VkFence ImageInFlight = VK_NULL_HANDLE;
};