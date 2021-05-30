#pragma once

#include "Tools/ResourceLoader.h"
#include "Structs/PipelineData.h"

#include <Vulkan/vulkan.h>
#include <vector>

class ShaderManager
{
public:
	ShaderManager(const VkDevice& r_VKDevice, const VkExtent2D& r_VKSwapChainExtent);
	~ShaderManager();

	void CreateDescriptorSetLayout(DescriptorData& r_DescriptorData);

	void CreatePipelineData(const VkRenderPass& r_RenderPass, PipeLineData& r_PipeLineData);
	void RecreatePipelines(const VkRenderPass& r_RenderPass);

	void CreateDescriptorPool(const size_t a_FrameAmount, DescriptorData& r_DescriptorData);
	void CreateDescriptorSet(const size_t a_FrameAmount, DescriptorData& r_DescriptorData, std::vector<VkBuffer>& r_ViewProjectionBuffers);

	//void RecreateDescriptors(const size_t a_FrameAmount, std::vector<VkBuffer>& r_ViewProjectionBuffers);


	VkShaderModule CreateShaderModule(const std::vector<char>& code);

private:
	void CreateGraphicsPipeline(const VkRenderPass& r_RenderPass, PipeLineData& r_PipeLineData);
	
	

	const VkDevice& rmvk_Device;
	const VkExtent2D& rmvk_SwapChainExtent;

	std::vector<PipeLineData*> p_SavedPipelines;
	std::vector<DescriptorData*> p_SavedDescriptors;
};