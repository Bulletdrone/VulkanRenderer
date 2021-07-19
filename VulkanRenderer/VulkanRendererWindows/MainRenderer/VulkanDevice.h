#pragma once

#include <Vulkan/vulkan.hpp>

#include <Handlers/CommandHandler.h>

class VulkanDevice
{
public:
	VulkanDevice(VkPhysicalDevice a_PhysicalDevice);
	~VulkanDevice();

	// Physical device representation
	VkPhysicalDevice m_PhysicalDevice;
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
	std::vector<const char*> m_SupportedExtensions;

	bool enableDebug = false;

	//Family Queue Indices
	struct
	{
		uint32_t graphics;
		uint32_t compute;
		uint32_t transfer;
	} queueFamilyIndices;

	operator VkDevice() const
	{
		return m_LogicalDevice;
	};

	uint32_t FindMemoryType(uint32_t a_TypeFilter, VkMemoryPropertyFlags a_Properties);
	QueueFamilyIndices GetQueueFamilies(VkPhysicalDevice a_Device);

};