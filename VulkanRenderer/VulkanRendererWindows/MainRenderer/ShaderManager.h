#pragma once

#include "Tools/ResourceLoader.h"
#include "Structs/PipelineData.h"
#include "Tools/ObjectPool.h"

#include "VulkanDevice.h"

class ShaderManager
{
public:
	ShaderManager(VulkanDevice& r_VulkanDevice, const VkExtent2D& r_VKSwapChainExtent);
	~ShaderManager();

	uint32_t CreateDescriptorSetLayout(TextureData* a_TextureData);

	uint32_t CreatePipelineData(const VkRenderPass& r_RenderPass, uint32_t a_DescriptorID);
	void RecreatePipelines(const VkRenderPass& r_RenderPass);

	void CreateDescriptorPool(const size_t a_FrameAmount, uint32_t a_DescID);
	void CreateDescriptorSet(const size_t a_FrameAmount, uint32_t a_DescID, std::vector<VkBuffer>& r_ViewProjectionBuffers, VkDeviceSize a_BufferSize);
	void RecreateDescriptors(const size_t a_FrameAmount, std::vector<VkBuffer>& r_ViewProjectionBuffers, VkDeviceSize a_BufferSize);

	VkShaderModule CreateShaderModule(const std::vector<char>& code);

public:
	ObjectPool<PipeLineData> PipelinePool{ ObjectPool<PipeLineData>(10, 5) };
	ObjectPool<DescriptorData> DescriptorPool{ ObjectPool<DescriptorData>(10, 5) };

private:
	void CreateGraphicsPipeline(const VkRenderPass& r_RenderPass, PipeLineData& r_PipeLineData);

	VulkanDevice& rm_VulkanDevice;
	const VkExtent2D& rmvk_SwapChainExtent;
};