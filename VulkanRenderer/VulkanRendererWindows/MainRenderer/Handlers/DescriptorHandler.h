#pragma once

#pragma warning (push, 0)
#include <Vulkan/vulkan.h>
#pragma warning (pop)

#include <vector>
#include <utility>
#include <unordered_map>
#include <algorithm>

class DescriptorLayoutCache
{
public:
	void Init(VkDevice a_Device);
	void Cleanup();

	VkDescriptorSetLayout CreateLayout(VkDescriptorSetLayoutCreateInfo* a_LayoutInfo);

	struct DescriptorLayoutInfo
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		bool operator==(const DescriptorLayoutInfo& a_Other) const;

		uint64_t hash() const;
	};

private:
	struct DescriptorLayoutHash 
	{
		uint64_t operator()(const DescriptorLayoutInfo& a_K) const
		{
			return a_K.hash();
		}
	};

	std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> layoutCache;
	VkDevice device;
};

class DescriptorAllocator
{
public:
	struct StandardPoolSize
	{
		std::vector<std::pair<VkDescriptorType, float>> standardSize =
		{
			//{ VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
			//{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
			//{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
			//{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
			//{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f }
			//{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
			//{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
			//{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
			//{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
		};
	};

	void ResetPools();
	bool AllocateSet(VkDescriptorSet* a_Set, VkDescriptorSetLayout a_Layout);

	void Init(VkDevice a_Device);

	void Cleanup();

	VkDevice device;

private:
	VkDescriptorPool GetPool();
	VkDescriptorPool CreatePool(uint32_t a_Count, VkDescriptorPoolCreateFlags a_Flags);

	VkDescriptorPool m_CurrentPool{ VK_NULL_HANDLE };
	StandardPoolSize m_DescriptorPoolSizes;
	std::vector<VkDescriptorPool> m_UsedPools;
	std::vector<VkDescriptorPool> m_FreePools;
};


class DescriptorBuilder
{
public:
	static DescriptorBuilder Begin(DescriptorLayoutCache* a_LayoutCache, DescriptorAllocator* a_Allocator);

	DescriptorBuilder& BindBuffer(uint32_t a_Binding, VkDescriptorBufferInfo* a_BufferInfo, VkDescriptorType a_Type, VkShaderStageFlags a_StageFlags);
	DescriptorBuilder& BindImage(uint32_t a_Binding, VkDescriptorImageInfo* a_ImageInfo, VkDescriptorType a_Type, VkShaderStageFlags a_StageFlags);

	bool Build(VkDescriptorSet& a_Set, VkDescriptorSetLayout& a_Layout);

private:
	std::vector<VkWriteDescriptorSet> m_Writes;
	std::vector<VkDescriptorSetLayoutBinding> m_Bindings;

	DescriptorLayoutCache* p_Cache = nullptr;
	DescriptorAllocator* p_Alloc = nullptr;
};