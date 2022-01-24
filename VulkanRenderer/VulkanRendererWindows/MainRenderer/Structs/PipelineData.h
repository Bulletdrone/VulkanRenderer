#pragma once
#pragma warning (push, 0)
#include <Vulkan/vulkan.h>
#pragma warning (pop)

#include <RenderObjects/Components/RenderObjectData.h>

struct PipeLineData
{
	uint32_t pipeID = 0;

	VkPipeline pipeLine = VK_NULL_HANDLE;
	VkPipelineLayout pipeLineLayout = VK_NULL_HANDLE;
};