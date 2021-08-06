#pragma once

#include <optional>

#include <Vulkan/vulkan.hpp>

#include "VulkanDebugger/VulkanDebug.h"

#include <Structs/BufferData.h>
#include "Window.h"

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct VulkanDevice
{
public:
	VulkanDevice();
	~VulkanDevice();

	//Deleted the vulkan components (CommandPool, LogicalDevice) from memory.
	void CleanupDevice();

	//Send nullptr to vulkanDebug if you do not want to enable debug.
	void VulkanDeviceSetup(VkPhysicalDevice a_PhysicalDevice, Window& a_Window, VulkanDebug* debug, size_t a_FrameBufferAmount);
	void CreateLogicalDevice(std::vector<const char*> a_EnabledExtensions, Window& r_Window, VkQueue& r_GraphicsQueue, VkQueue& r_PresentQueue);

	// Physical device representation
	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	// Logical device representation (application's view of the device)
	VkDevice m_LogicalDevice;
	// Properties of the physical device including limits that the application can check against
	VkPhysicalDeviceProperties m_Properties;
	// Features of the physical device that an application can use to check if a feature is supported
	VkPhysicalDeviceFeatures m_Features;
	// Features that have been enabled for use on the physical device
	VkPhysicalDeviceFeatures m_EnabledFeatures;
	// Memory types and heaps of the physical device
	VkPhysicalDeviceMemoryProperties m_MemoryProperties;
	// Queue family properties of the physical device
	std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
	// List of extensions supported by the device
	std::vector<std::string> m_SupportedExtensions;

	// Default command pool for the graphics queue family index.
	VkCommandPool m_CommandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> m_DeviceCommandBuffer;

	//Temp Values, likely to be placed somewhere else.
	float AnisotropyFilteringMax = 0;
	VkQueue* p_GraphicsQueue;

	bool m_EnableDebug = false;
	VulkanDebug* DE_VulkanDebug = nullptr;

	operator VkDevice() const
	{
		return m_LogicalDevice;
	};

	uint32_t FindMemoryType(uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties);

	//CommandBuffer Functions
	void CreateCommandPools(QueueFamilyIndices a_QueueFamilyIndices);
	void FreeCommandPool();

	void ClearPreviousCommand(size_t a_Frame);
	VkCommandBuffer& CreateAndBeginCommand(size_t a_Frame, uint32_t a_QueueFamilyIndex, VkRenderPass& r_RenderPass, VkFramebuffer& r_SwapChainFrameBuffer, VkExtent2D& r_SwapChainExtent);
	void EndCommand(VkCommandBuffer& r_CommandBuffer);

	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer a_CommandBuffer);

	VkCommandPoolCreateInfo CreateCommandPoolInfo(uint32_t a_QueueFamilyIndex, VkCommandPoolCreateFlags a_Flags);
	VkCommandBufferAllocateInfo CreateCommandBufferInfo(VkCommandPool& r_Pool, uint32_t a_Count, VkCommandBufferLevel a_Level);

	//Buffer Functions
	void CreateVertexBuffers(BufferData<Vertex>* a_VertexData);
	void CreateIndexBuffers(BufferData<uint32_t>* a_IndexData);
	void CreateUniformBuffers(std::vector<VkBuffer>& r_UniformBuffers, std::vector<VkDeviceMemory>& r_UniformBuffersMemory, const size_t a_SwampChainSize);
	//Standard Buffer Creation
	void CreateBuffer(VkDeviceSize a_Size, VkBufferUsageFlags a_Usage, VkMemoryPropertyFlags a_Properties, VkBuffer& r_Buffer, VkDeviceMemory& r_BufferMemory);
	void CopyBuffer(VkDeviceSize a_Size, VkBuffer& r_SrcBuffer, VkBuffer& r_DstBuffer);

private:
	bool ExtentionSupported(const std::string& a_Extension);
};

//Finding Queuefamily
inline QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice& r_PhysicalDevice, Window& a_Window)
{
	QueueFamilyIndices t_Indices;

	uint32_t t_QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(r_PhysicalDevice, &t_QueueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> t_QueueFamilies(t_QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(r_PhysicalDevice, &t_QueueFamilyCount, t_QueueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : t_QueueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			t_Indices.graphicsFamily = i;
			VkBool32 t_PresentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(r_PhysicalDevice, i, a_Window.GetSurface(), &t_PresentSupport);

			if (t_PresentSupport)
			{
				t_Indices.presentFamily = i;
			}
		}

		if (t_Indices.isComplete())
			break;

		i++;
	}

	return t_Indices;
}