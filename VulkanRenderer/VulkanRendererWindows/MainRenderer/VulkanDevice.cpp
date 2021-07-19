#include "VulkanDevice.h"

#include <assert.h>

VulkanDevice::VulkanDevice(VkPhysicalDevice a_PhysicalDevice)
{
	m_PhysicalDevice = a_PhysicalDevice;

	//Get the device properties like the limits.
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);

	vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);

	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);

	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
	assert(queueFamilyCount > 0); //If this is smaller then 1 then abort.
	m_QueueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, m_QueueFamilyProperties.data());

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
}

VulkanDevice::~VulkanDevice()
{
	//Destroy handlers.

	if (m_LogicalDevice)
	{
		vkDestroyDevice(m_LogicalDevice, nullptr);
	}
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

QueueFamilyIndices VulkanDevice::GetQueueFamilies(VkPhysicalDevice a_Device)
{
	QueueFamilyIndices t_Indices;

	uint32_t t_QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(a_Device, &t_QueueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> t_QueueFamilies(t_QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(a_Device, &t_QueueFamilyCount, t_QueueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : t_QueueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			t_Indices.graphicsFamily = i;
			VkBool32 t_PresentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(a_Device, i, rm_Window->GetSurface(), &t_PresentSupport);

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