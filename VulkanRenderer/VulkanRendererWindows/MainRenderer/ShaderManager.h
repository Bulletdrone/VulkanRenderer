#pragma once

#include "Tools/ResourceLoader.h"
#include "Structs/PipelineData.h"
#include "Tools/ObjectPool.h"

#include "Handlers/DescriptorHandler.h"

#include "VulkanDevice.h"

class ShaderManager
{
public:
	ShaderManager(VulkanDevice& r_VulkanDevice, const VkExtent2D& r_VKSwapChainExtent);
	~ShaderManager();

	uint32_t CreatePipelineData(const VkRenderPass& r_RenderPass, std::vector<VkDescriptorSetLayout>& a_DescriptorSetLayouts);

	////Recreation needs to be done from the ground up again, these are just here as a reminder.
	//void RecreatePipelines(const VkRenderPass& r_RenderPass);
	//void RecreateDescriptors(const size_t a_FrameAmount, std::vector<VkBuffer>& r_UniformBuffers, VkDeviceSize a_BufferSize);

	VkShaderModule CreateShaderModule(const std::vector<char>& code);

public:
	ObjectPool<PipeLineData> PipelinePool{ ObjectPool<PipeLineData>() };

private:
	VulkanDevice& rm_VulkanDevice;
	const VkExtent2D& rmvk_SwapChainExtent;
};