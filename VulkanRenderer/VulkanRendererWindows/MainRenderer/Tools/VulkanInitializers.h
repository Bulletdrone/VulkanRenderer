#pragma once
#pragma warning (push, 0)
#include <Vulkan/vulkan.h>
#pragma warning (pop)

namespace VkInit
{
	inline VkDescriptorBufferInfo CreateDescriptorBufferInfo(VkBuffer& r_UniformBuffer, VkDeviceSize a_Offset, VkDeviceSize a_BufferSize)
	{
		VkDescriptorBufferInfo t_BufferInfo{};
		t_BufferInfo.buffer = r_UniformBuffer;
		t_BufferInfo.offset = a_Offset;
		t_BufferInfo.range = a_BufferSize;

		return t_BufferInfo;
	}

	inline VkDescriptorImageInfo CreateDescriptorImageInfo(VkImageLayout a_ImageLayout, VkImageView& r_ImageView, VkSampler& r_Sampler)
	{
		VkDescriptorImageInfo t_ImageInfo{};
		t_ImageInfo.imageLayout = a_ImageLayout;
		t_ImageInfo.imageView = r_ImageView;
		t_ImageInfo.sampler = r_Sampler;

		return t_ImageInfo;
	}


	//Layout Creation.
	inline VkDescriptorSetLayoutBinding CreateDescriptorSetLayoutBinding(VkDescriptorType a_Type, VkShaderStageFlags a_Flags, uint32_t a_Binding)
	{
		VkDescriptorSetLayoutBinding t_LayoutBinding{};
		t_LayoutBinding.binding = a_Binding;
		t_LayoutBinding.descriptorType = a_Type;
		t_LayoutBinding.descriptorCount = 1;
		t_LayoutBinding.stageFlags = a_Flags;
		t_LayoutBinding.pImmutableSamplers = nullptr; // Optional

		return t_LayoutBinding;
	}

	//Shader Creation
	inline VkShaderModuleCreateInfo CreateShaderModule(const unsigned char* a_ShaderCode, const size_t a_CodeSize)
	{
		VkShaderModuleCreateInfo t_CreateInfo{};
		t_CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		t_CreateInfo.codeSize = a_CodeSize;
		t_CreateInfo.pCode = reinterpret_cast<const uint32_t*>(a_ShaderCode);

		return t_CreateInfo;
	}
}