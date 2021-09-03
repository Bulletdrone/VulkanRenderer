#include "VulkanDevice.h"

#include <assert.h>
#include <set>
#include <iostream>

VulkanDevice::VulkanDevice()
{

}

VulkanDevice::~VulkanDevice()
{

}

void VulkanDevice::CleanupDevice()
{
    if (m_CommandPool)
    {
        vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);
    }

    if (m_LogicalDevice)
    {
        vkDestroyDevice(m_LogicalDevice, nullptr);
    }
}

void VulkanDevice::VulkanDeviceSetup(VkPhysicalDevice a_PhysicalDevice, Window& a_Window, VulkanDebug* debug, size_t a_FrameBufferAmount)
{
    m_DeviceCommandBuffer.resize(a_FrameBufferAmount);

    if (debug != nullptr)
    {
        m_EnableDebug = true;
        DE_VulkanDebug = debug;
    }

	m_PhysicalDevice = a_PhysicalDevice;

	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);
    //Set the Anisotropy filtering here for now, later this is a option.
    AnisotropyFilteringMax = m_Properties.limits.maxSamplerAnisotropy;

	vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);

    //FOR NOW ENABLE ALL FEATURES
    m_EnabledFeatures = m_Features;

	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);

	//Set queue families for requested queues. 
    uint32_t t_QueueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &t_QueueFamilyCount, nullptr);
    assert(t_QueueFamilyCount > 0);
    m_QueueFamilyProperties.resize(t_QueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &t_QueueFamilyCount, m_QueueFamilyProperties.data());

	//Set Extension support.
	uint32_t extCount = 0;
	vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extCount, nullptr);
	if (extCount > 0)
	{
		std::vector<VkExtensionProperties> extensions(extCount);
		if (vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
		{
			for (auto ext : extensions)
			{
				m_SupportedExtensions.push_back(ext.extensionName);
			}
		}
	}
    

    //For now explicitally add that it will present to the screen using the swapchain.
    //m_SupportedExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void VulkanDevice::CreateLogicalDevice(std::vector<const char*> a_EnabledExtensions, Window& r_Window, VkQueue& r_GraphicsQueue, VkQueue& r_PresentQueue)
{
    p_GraphicsQueue = &r_GraphicsQueue;
    QueueFamilyIndices t_Indices = FindQueueFamilies(m_PhysicalDevice, r_Window);

    std::vector<VkDeviceQueueCreateInfo> t_QueueCreateInfos;
    std::set<uint32_t> t_UniqueQueueFamilies = { t_Indices.graphicsFamily.value(), t_Indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : t_UniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        t_QueueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo t_CreateInfo{};
    t_CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    t_CreateInfo.queueCreateInfoCount = static_cast<uint32_t>(t_QueueCreateInfos.size());
    t_CreateInfo.pQueueCreateInfos = t_QueueCreateInfos.data();

    t_CreateInfo.pEnabledFeatures = &m_EnabledFeatures;

    if (a_EnabledExtensions.size() > 0)
    {
        for (const char* t_EnabledExtension : a_EnabledExtensions)
        {
            if (!ExtentionSupported(t_EnabledExtension)) {
                std::cerr << "Enabled device extension \"" << t_EnabledExtension << "\" is not present at device level\n";
            }
        }
    }

    a_EnabledExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    t_CreateInfo.enabledExtensionCount = (uint32_t)a_EnabledExtensions.size();
    t_CreateInfo.ppEnabledExtensionNames = a_EnabledExtensions.data();

    if (m_EnableDebug)
    {
        DE_VulkanDebug->AddDebugLayerInstanceDevice(t_CreateInfo);
    }
    else
    {
        t_CreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_PhysicalDevice, &t_CreateInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_LogicalDevice, t_Indices.graphicsFamily.value(), 0, &r_GraphicsQueue);
    vkGetDeviceQueue(m_LogicalDevice, t_Indices.presentFamily.value(), 0, &r_PresentQueue);
}

//PRIVATE FUNCTION TO FIND MEMORYTYPE
uint32_t VulkanDevice::FindMemoryType(uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((a_TypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & a_Properties) == a_Properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

//Thanks to VkGuide.dev
VkDeviceSize VulkanDevice::AllignUniformBufferSize(VkDeviceSize a_BufferSize)
{
    VkDeviceSize minSize = m_Properties.limits.minUniformBufferOffsetAlignment;
    VkDeviceSize alignedSize = a_BufferSize;

    if (minSize > 0)
    {
        alignedSize = (alignedSize + minSize - 1) & ~(minSize - 1);
    }

    return alignedSize;
}

#pragma region Buffer Helpers

void VulkanDevice::CreateCommandPools(QueueFamilyIndices a_QueueFamilyIndices)
{
    VkCommandPoolCreateInfo t_PoolInfo = CreateCommandPoolInfo(a_QueueFamilyIndices.graphicsFamily.value(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    if (vkCreateCommandPool(m_LogicalDevice, &t_PoolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void VulkanDevice::FreeCommandPool()
{
    vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, static_cast<uint32_t>(m_DeviceCommandBuffer.size()), m_DeviceCommandBuffer.data());
}

void VulkanDevice::ClearPreviousCommand(size_t a_Frame)
{
    vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &m_DeviceCommandBuffer[a_Frame]);
}

VkCommandBuffer& VulkanDevice::CreateAndBeginCommand(size_t a_Frame, uint32_t a_QueueFamilyIndex, VkRenderPass& r_RenderPass, VkFramebuffer& r_SwapChainFrameBuffer, VkExtent2D& r_SwapChainExtent)
{
    VkCommandBuffer& commandBuffer = m_DeviceCommandBuffer[a_Frame];

    VkCommandPoolCreateInfo t_CommandPoolInfo = CreateCommandPoolInfo(a_QueueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    //allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo t_CmdAllocInfo = CreateCommandBufferInfo(m_CommandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    if (vkAllocateCommandBuffers(m_LogicalDevice, &t_CmdAllocInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }

    VkCommandBufferBeginInfo t_BeginInfo{};
    t_BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    t_BeginInfo.flags = 0; // Optional
    t_BeginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &t_BeginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo t_RenderPassInfo{};
    t_RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    t_RenderPassInfo.renderPass = r_RenderPass;
    t_RenderPassInfo.framebuffer = r_SwapChainFrameBuffer;

    t_RenderPassInfo.renderArea.offset = { 0, 0 };
    t_RenderPassInfo.renderArea.extent = r_SwapChainExtent;

    std::array<VkClearValue, 2> t_ClearValues{};
    t_ClearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
    t_ClearValues[1].depthStencil = { 1.0f, 0 };

    t_RenderPassInfo.clearValueCount = static_cast<uint32_t>(t_ClearValues.size());;
    t_RenderPassInfo.pClearValues = t_ClearValues.data();


    ////Begin the drawing.
    vkCmdBeginRenderPass(commandBuffer, &t_RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    return commandBuffer;
}

//finalize the render pass
void VulkanDevice::EndCommand(VkCommandBuffer& r_CommandBuffer)
{
    vkCmdEndRenderPass(r_CommandBuffer);

    if (vkEndCommandBuffer(r_CommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

VkCommandBuffer VulkanDevice::BeginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo t_AllocInfo = CreateCommandBufferInfo(m_CommandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    VkCommandBuffer t_CommandBuffer;
    vkAllocateCommandBuffers(m_LogicalDevice, &t_AllocInfo, &t_CommandBuffer);

    VkCommandBufferBeginInfo t_BeginInfo{};
    t_BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    t_BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(t_CommandBuffer, &t_BeginInfo);

    return t_CommandBuffer;
}

void VulkanDevice::EndSingleTimeCommands(VkCommandBuffer a_CommandBuffer)
{
    vkEndCommandBuffer(a_CommandBuffer);

    VkSubmitInfo t_SubmitInfo{};
    t_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    t_SubmitInfo.commandBufferCount = 1;
    t_SubmitInfo.pCommandBuffers = &a_CommandBuffer;

    vkQueueSubmit(*p_GraphicsQueue, 1, &t_SubmitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(*p_GraphicsQueue);

    vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &a_CommandBuffer);
}

VkCommandPoolCreateInfo VulkanDevice::CreateCommandPoolInfo(uint32_t a_QueueFamilyIndex, VkCommandPoolCreateFlags a_Flags)
{
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;

    info.queueFamilyIndex = a_QueueFamilyIndex;
    info.flags = a_Flags;
    return info;
}

VkCommandBufferAllocateInfo VulkanDevice::CreateCommandBufferInfo(VkCommandPool& r_Pool, uint32_t a_Count, VkCommandBufferLevel a_Level)
{
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;

    info.commandPool = r_Pool;
    info.commandBufferCount = a_Count;
    info.level = a_Level;
    return info;
}

#pragma endregion

#pragma region Specific Buffer Creation

void VulkanDevice::CreateVertexBuffers(BufferData<Vertex>* a_VertexData)
{
    VkDeviceSize t_BufferSize = a_VertexData->CreateBufferSize();

    VkBuffer t_StagingBuffer;
    VkDeviceMemory t_StagingBufferMemory;
    CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        t_StagingBuffer, t_StagingBufferMemory);

    void* t_Data;
    vkMapMemory(m_LogicalDevice, t_StagingBufferMemory, 0, t_BufferSize, 0, &t_Data);
    memcpy(t_Data, a_VertexData->GetElements().data(), (size_t)t_BufferSize);
    vkUnmapMemory(m_LogicalDevice, t_StagingBufferMemory);

    CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        a_VertexData->GetBuffer(), a_VertexData->GetBufferMemory());

    CopyBuffer(t_BufferSize, t_StagingBuffer, a_VertexData->GetBuffer());

    vkDestroyBuffer(m_LogicalDevice, t_StagingBuffer, nullptr);
    vkFreeMemory(m_LogicalDevice, t_StagingBufferMemory, nullptr);
}

void VulkanDevice::CreateIndexBuffers(BufferData<uint32_t>* a_IndexData)
{
    VkDeviceSize t_BufferSize = a_IndexData->CreateBufferSize();

    VkBuffer t_StagingBuffer;
    VkDeviceMemory t_StagingBufferMemory;
    CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        t_StagingBuffer, t_StagingBufferMemory);

    void* t_Data;
    vkMapMemory(m_LogicalDevice, t_StagingBufferMemory, 0, t_BufferSize, 0, &t_Data);
    memcpy(t_Data, a_IndexData->GetElements().data(), (size_t)t_BufferSize);
    vkUnmapMemory(m_LogicalDevice, t_StagingBufferMemory);

    CreateBuffer(t_BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        a_IndexData->GetBuffer(), a_IndexData->GetBufferMemory());

    CopyBuffer(t_BufferSize, t_StagingBuffer, a_IndexData->GetBuffer());

    vkDestroyBuffer(m_LogicalDevice, t_StagingBuffer, nullptr);
    vkFreeMemory(m_LogicalDevice, t_StagingBufferMemory, nullptr);
}

void VulkanDevice::CreateUniformBuffers(std::vector<VkBuffer>& r_UniformBuffers, std::vector<VkDeviceMemory>& r_UniformBuffersMemory, const size_t a_SwampChainSize, VkDeviceSize a_BufferSize)
{
    //VkDeviceSize t_BufferSize = sizeof(ViewProjection);

    r_UniformBuffers.resize(a_SwampChainSize);
    r_UniformBuffersMemory.resize(a_SwampChainSize);

    for (size_t i = 0; i < a_SwampChainSize; i++)
    {
        CreateBuffer(a_BufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            r_UniformBuffers[i], r_UniformBuffersMemory[i]);
    }

   // VkDeviceSize t_UniBufferSize = sizeof(ViewProjection);

    //r_UniformBuffers.resize(a_SwampChainSize);
    //r_UniformBuffersMemory.resize(a_SwampChainSize);

    //for (size_t i = 0; i < a_SwampChainSize; i++)
    //{
    //    CreateBuffer(a_BufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    //        r_UniformBuffers[i], r_UniformBuffersMemory[i]);
    //}
}

    

//PRIVATES

#pragma region Buffer Helpers

void VulkanDevice::CreateBuffer(VkDeviceSize a_Size, VkBufferUsageFlags a_Usage, VkMemoryPropertyFlags a_Properties, VkBuffer& r_Buffer, VkDeviceMemory& r_BufferMemory)
{
    VkBufferCreateInfo t_BufferInfo{};
    t_BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    t_BufferInfo.size = a_Size;
    t_BufferInfo.usage = a_Usage;
    t_BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkResult t;
    if ((t = vkCreateBuffer(m_LogicalDevice, &t_BufferInfo, nullptr, &r_Buffer)) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements t_MemRequirements;
    vkGetBufferMemoryRequirements(m_LogicalDevice, r_Buffer, &t_MemRequirements);

    VkMemoryAllocateInfo t_AllocInfo{};
    t_AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    t_AllocInfo.allocationSize = t_MemRequirements.size;
    t_AllocInfo.memoryTypeIndex = FindMemoryType(t_MemRequirements.memoryTypeBits, a_Properties);

    if (vkAllocateMemory(m_LogicalDevice, &t_AllocInfo, nullptr, &r_BufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(m_LogicalDevice, r_Buffer, r_BufferMemory, 0);
}

void VulkanDevice::CopyBuffer(VkDeviceSize a_Size, VkBuffer& r_SrcBuffer, VkBuffer& r_DstBuffer)
{
    VkCommandBuffer t_CommandBuffer = BeginSingleTimeCommands();

    VkBufferCopy t_CopyRegion{};
    t_CopyRegion.size = a_Size;
    vkCmdCopyBuffer(t_CommandBuffer, r_SrcBuffer, r_DstBuffer, 1, &t_CopyRegion);

    EndSingleTimeCommands(t_CommandBuffer);
}

#pragma endregion

bool VulkanDevice::ExtentionSupported(const std::string& a_Extension)
{
    return (std::find(m_SupportedExtensions.begin(), m_SupportedExtensions.end(), a_Extension) != m_SupportedExtensions.end());
}