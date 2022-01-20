#include "DescriptorHandler.h"

#include <stdexcept>

#pragma region DescriptorLayoutCache

void DescriptorLayoutCache::Init(VkDevice a_Device)
{
	device = a_Device;
}

void DescriptorLayoutCache::Cleanup()
{
	for (auto pair : layoutCache)
	{
		vkDestroyDescriptorSetLayout(device, pair.second, nullptr);
	}
}

VkDescriptorSetLayout DescriptorLayoutCache::CreateLayout(VkDescriptorSetLayoutCreateInfo* a_LayoutInfo)
{
	DescriptorLayoutInfo t_LayoutInfo;
	t_LayoutInfo.bindings.reserve(a_LayoutInfo->bindingCount);

	bool isSorted = true;
	int lastBinding = -1;

	for (size_t i = 0; i < a_LayoutInfo->bindingCount; i++)
	{
		t_LayoutInfo.bindings.push_back(a_LayoutInfo->pBindings[i]);

		//Check that the bindings are in strict increasing order.
		if (static_cast<int>(a_LayoutInfo->pBindings[i].binding) > lastBinding)
		{
			lastBinding = static_cast<int>(a_LayoutInfo->pBindings[i].binding);
		}
		else
		{
			isSorted = false;
		}

		//sort the bindings if they aren't in order.
		if (!isSorted)
		{
			std::sort(t_LayoutInfo.bindings.begin(), t_LayoutInfo.bindings.end(), [](VkDescriptorSetLayoutBinding& a, VkDescriptorSetLayoutBinding& b)
				{
					return a.binding < b.binding;
				});
		}
	}

	auto it = layoutCache.find(t_LayoutInfo);
	if (it != layoutCache.end())
	{
		return (*it).second;
	}
	else
	{
		//create a new one (not found)
		VkDescriptorSetLayout layout;
		vkCreateDescriptorSetLayout(device, a_LayoutInfo, nullptr, &layout);

		//add to cache
		layoutCache[t_LayoutInfo] = layout;
		return layout;
	}
}

bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& a_Other) const
{
	if (a_Other.bindings.size() != bindings.size())
	{
		return false;
	}
	else {
		//compare each of the bindings is the same. Bindings are sorted so they will match
		for (int i = 0; i < bindings.size(); i++)
		{
			if (a_Other.bindings[i].binding != bindings[i].binding)
			{
				return false;
			}
			if (a_Other.bindings[i].descriptorType != bindings[i].descriptorType)
			{
				return false;
			}
			if (a_Other.bindings[i].descriptorCount != bindings[i].descriptorCount)
			{
				return false;
			}
			if (a_Other.bindings[i].stageFlags != bindings[i].stageFlags)
			{
				return false;
			}
		}
		return true;
	}
}

size_t DescriptorLayoutCache::DescriptorLayoutInfo::hash() const 
{
	using std::size_t;
	using std::hash;

	size_t result = hash<size_t>()(bindings.size());

	for (const VkDescriptorSetLayoutBinding& b : bindings)
	{
		//pack the binding data into a single int64. Not fully correct but it's ok
		uint64_t bindingHash = static_cast<uint64_t>(b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24);

		//shuffle the packed binding data and xor it with the main hash
		result ^= hash<size_t>()(bindingHash);
	}

	return result;
}

#pragma endregion

#pragma region DescriptorAllocator

void DescriptorAllocator::ResetPools()
{
	for (auto p : m_UsedPools)
	{
		vkResetDescriptorPool(device, p, 0);
		m_FreePools.push_back(p);
	}

	m_UsedPools.clear();

	m_CurrentPool = VK_NULL_HANDLE;
}

bool DescriptorAllocator::AllocateSet(VkDescriptorSet* a_Set, VkDescriptorSetLayout a_Layout)
{
	if (m_CurrentPool == VK_NULL_HANDLE)
	{
		m_CurrentPool = GetPool();
		m_UsedPools.push_back(m_CurrentPool);
	}

	VkDescriptorSetAllocateInfo t_AllocInfo = {};
	t_AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	t_AllocInfo.pNext = nullptr;

	t_AllocInfo.pSetLayouts = &a_Layout;
	t_AllocInfo.descriptorPool = m_CurrentPool;
	t_AllocInfo.descriptorSetCount = 1;

	VkResult t_AllocResult = vkAllocateDescriptorSets(device, &t_AllocInfo, a_Set);
	bool t_NeedReallocate = false;

	switch (t_AllocResult)
	{
	case VK_SUCCESS:
		//Allocate success.
		return true;
		break;

	case VK_ERROR_FRAGMENTED_POOL:
	case VK_ERROR_OUT_OF_POOL_MEMORY:
		//need a new pool.
		t_NeedReallocate = true;
		break;

	default:
		//Severe allocate Error
		return false;
		break;
	}

	if (t_NeedReallocate)
	{
		//Allocate a new pool.
		m_CurrentPool = GetPool();
		m_UsedPools.push_back(m_CurrentPool);

		t_AllocResult = vkAllocateDescriptorSets(device, &t_AllocInfo, a_Set);

		//If it fails again then there is something really wrong.
		if (t_AllocResult == VK_SUCCESS)
		{
			return true;
		}
	}

	return false;
}

void DescriptorAllocator::Init(VkDevice a_Device)
{
	device = a_Device;
}

void DescriptorAllocator::Cleanup()
{
	for (size_t i = 0; i < m_FreePools.size(); i++)
	{
		vkDestroyDescriptorPool(device, m_FreePools[i], nullptr);
	}
	for (size_t i = 0; i < m_UsedPools.size(); i++)
	{
		vkDestroyDescriptorPool(device, m_UsedPools[i], nullptr);
	}
}

VkDescriptorPool DescriptorAllocator::GetPool()
{
	//there are reusable pools availible
	if (m_FreePools.size() > 0)
	{
		//grab pool from the back of the vector and remove it from there.
		VkDescriptorPool pool = m_FreePools.back();
		m_FreePools.pop_back();
		return pool;
	}
	else
	{
		//no pools availible, so create a new one
		return CreatePool(1000, 0);
	}
}

VkDescriptorPool DescriptorAllocator::CreatePool(uint32_t a_Count, VkDescriptorPoolCreateFlags a_Flags)
{
	std::vector<VkDescriptorPoolSize> t_Sizes;
	t_Sizes.reserve(m_DescriptorPoolSizes.standardSize.size());

	for (auto sz : m_DescriptorPoolSizes.standardSize)
	{
		t_Sizes.push_back({ sz.first, uint32_t(sz.second * a_Count) });
	}

	VkDescriptorPoolCreateInfo t_PoolSize = {};
	t_PoolSize.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	t_PoolSize.flags = a_Flags;
	t_PoolSize.maxSets = a_Count;
	t_PoolSize.poolSizeCount = (uint32_t)t_Sizes.size();
	t_PoolSize.pPoolSizes = t_Sizes.data();

	VkDescriptorPool t_DescriptorPool;
	vkCreateDescriptorPool(device, &t_PoolSize, nullptr, &t_DescriptorPool);

	return t_DescriptorPool;
}

#pragma endregion

#pragma region DescriptorBuilder

DescriptorBuilder DescriptorBuilder::Begin(DescriptorLayoutCache* a_LayoutCache, DescriptorAllocator* a_Allocator)
{
	DescriptorBuilder builder;

	builder.p_Cache = a_LayoutCache;
	builder.p_Alloc = a_Allocator;
	return builder;
}

DescriptorBuilder& DescriptorBuilder::BindBuffer(uint32_t a_Binding, VkDescriptorBufferInfo* a_BufferInfo, VkDescriptorType a_Type, VkShaderStageFlags a_StageFlags)
{
	VkDescriptorSetLayoutBinding t_NewBinding{};

	t_NewBinding.descriptorCount = 1;
	t_NewBinding.descriptorType = a_Type;
	t_NewBinding.pImmutableSamplers = nullptr;
	t_NewBinding.stageFlags = a_StageFlags;
	t_NewBinding.binding = a_Binding;

	m_Bindings.push_back(t_NewBinding);

	//Create the descriptorwrite.
	VkWriteDescriptorSet t_NewWrite{};
	t_NewWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	t_NewWrite.pNext = nullptr;

	t_NewWrite.descriptorCount = 1;
	t_NewWrite.descriptorType = a_Type;
	t_NewWrite.pBufferInfo = a_BufferInfo;
	t_NewWrite.dstBinding = a_Binding;

	m_Writes.push_back(t_NewWrite);
	return *this;
}

DescriptorBuilder& DescriptorBuilder::BindImage(uint32_t a_Binding, VkDescriptorImageInfo* a_ImageInfo, VkDescriptorType a_Type, VkShaderStageFlags a_StageFlags)
{
	VkDescriptorSetLayoutBinding t_NewBinding{};

	t_NewBinding.descriptorCount = 1;
	t_NewBinding.descriptorType = a_Type;
	t_NewBinding.pImmutableSamplers = nullptr;
	t_NewBinding.stageFlags = a_StageFlags;
	t_NewBinding.binding = a_Binding;

	m_Bindings.push_back(t_NewBinding);

	//Create the descriptorwrite.
	VkWriteDescriptorSet t_NewWrite{};
	t_NewWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	t_NewWrite.pNext = nullptr;

	t_NewWrite.descriptorCount = 1;
	t_NewWrite.descriptorType = a_Type;
	t_NewWrite.pImageInfo = a_ImageInfo;
	t_NewWrite.dstBinding = a_Binding;

	m_Writes.push_back(t_NewWrite);
	return *this;
}

bool DescriptorBuilder::Build(VkDescriptorSet& a_Set, VkDescriptorSetLayout& a_Layout)
{
	VkDescriptorSetLayoutCreateInfo t_LayoutInfo{};
	t_LayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	t_LayoutInfo.pNext = nullptr;

	t_LayoutInfo.pBindings = m_Bindings.data();
	t_LayoutInfo.bindingCount = static_cast<uint32_t>(m_Bindings.size());

	p_Cache->CreateLayout(&t_LayoutInfo);

	//allocate descriptor.
	bool t_Success = p_Alloc->AllocateSet(&a_Set, a_Layout);
	if (!t_Success) { return false; }

	//Write Descriptor.
	for (VkWriteDescriptorSet& w : m_Writes)
	{
		w.dstSet = a_Set;
	}

	vkUpdateDescriptorSets(p_Alloc->device, static_cast<uint32_t>(m_Writes.size()), m_Writes.data(), 0, nullptr);


	return true;
}

#pragma endregion
