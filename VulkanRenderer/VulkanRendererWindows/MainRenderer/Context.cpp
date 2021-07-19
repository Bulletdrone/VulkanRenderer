#include "Context.h"

#include <iostream>
#include <set>

Context::Context(bool a_bDebugEnabled, Window* p_Window)
{
	if (a_bDebugEnabled)
	{
		m_bDebugEnabled = a_bDebugEnabled;
		DE_VulkanDebug = new VulkanDebug(mvk_Instance);
	}
}

Context::~Context()
{
	CleanupContext();
}

void Context::CleanupContext()
{
	vkDestroyDevice(mvk_Device, nullptr);

	if (m_bDebugEnabled)
		delete DE_VulkanDebug;

	vkDestroySurfaceKHR(mvk_Instance, rm_Window->GetSurface(), nullptr);
	vkDestroyInstance(mvk_Instance, nullptr);
}

#pragma region CreationFunctions.

void Context::CreateVKInstance(const ContextData& r_ContextData)
{
	//If debug is enabled check if the layers are supported.
	if (m_bDebugEnabled && !DE_VulkanDebug->CheckValidationLayerSupport())
		throw std::runtime_error("validation layers requested, but not available!");


	//Setting up Vulkan: ApplicationInfo.
	VkApplicationInfo t_AppInfo{};
	t_AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	t_AppInfo.pApplicationName = "Zeep's VulkanTest";
	t_AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	t_AppInfo.pEngineName = "No Engine";
	t_AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	t_AppInfo.apiVersion = VK_API_VERSION_1_0;

	//Setting up Vulkan: VKInstance Setup
	VkInstanceCreateInfo t_CreateInfo{};
	t_CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	t_CreateInfo.pApplicationInfo = &t_AppInfo;

	t_CreateInfo.enabledExtensionCount = static_cast<uint32_t>(r_ContextData.WindowExtentions.size());
	t_CreateInfo.ppEnabledExtensionNames = r_ContextData.WindowExtentions.data();

	//Set the Debug Layers if debug is enabled, otherwise set it to 0
	if (m_bDebugEnabled)
	{
		DE_VulkanDebug->AddDebugLayerInstance(t_CreateInfo);

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		DE_VulkanDebug->PopulateDebugStartupMessengerCreateInfo(debugCreateInfo);
		t_CreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		t_CreateInfo.enabledLayerCount = 0;
		t_CreateInfo.pNext = nullptr;
	}

	//Create and check if the Instance is a success.
	if (vkCreateInstance(&t_CreateInfo, nullptr, &mvk_Instance) != VK_SUCCESS)
	{
		throw printf("Vulkan not successfully Instanced.");
	}

	//Setup messenger if debug is enabled.
	if (m_bDebugEnabled)
		DE_VulkanDebug->SetupDebugMessanger(nullptr);
}

void Context::PickPhysicalDevice()
{
	uint32_t t_DeviceCount = 0;
	vkEnumeratePhysicalDevices(mvk_Instance, &t_DeviceCount, nullptr);

	//No devices that support Vulcan.
	if (t_DeviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> t_Devices(t_DeviceCount);
	vkEnumeratePhysicalDevices(mvk_Instance, &t_DeviceCount, t_Devices.data());

	for (const VkPhysicalDevice& device : t_Devices)
	{
		if (IsDeviceSuitable(device))
		{
			VkPhysicalDeviceProperties t_PhysDeviceProperties;
			vkGetPhysicalDeviceProperties(device, &t_PhysDeviceProperties);
			if (t_PhysDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				mvk_PhysicalDevice = device;
				break;
			}
		}
	}

	if (mvk_PhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

void Context::CreateLogicalDevice(VkQueue& vkr_GraphicsQueue, VkQueue& vkr_PresentQueue)
{
	QueueFamilyIndices t_Indices = FindQueueFamilies(mvk_PhysicalDevice);

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

	VkPhysicalDeviceFeatures t_DeviceFeatures{};
	t_DeviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo t_CreateInfo{};
	t_CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	t_CreateInfo.queueCreateInfoCount = static_cast<uint32_t>(t_QueueCreateInfos.size());
	t_CreateInfo.pQueueCreateInfos = t_QueueCreateInfos.data();

	t_CreateInfo.pEnabledFeatures = &t_DeviceFeatures;

	t_CreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	t_CreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

	if (m_bDebugEnabled)
	{
		DE_VulkanDebug->AddDebugLayerInstanceDevice(t_CreateInfo);
	}
	else
	{
		t_CreateInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(mvk_PhysicalDevice, &t_CreateInfo, nullptr, &mvk_Device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(mvk_Device, t_Indices.graphicsFamily.value(), 0, &vkr_GraphicsQueue);
	vkGetDeviceQueue(mvk_Device, t_Indices.presentFamily.value(), 0, &vkr_PresentQueue);
}

#pragma endregion

SwapChainSupportDetails Context::QuerySwapChainSupport(VkPhysicalDevice a_Device) {
	SwapChainSupportDetails t_Details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(a_Device, rm_Window->GetSurface(), &t_Details.capabilities);

	//Get Format Data
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(a_Device, rm_Window->GetSurface(), &formatCount, nullptr);

	if (formatCount != 0)
	{
		t_Details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(a_Device, rm_Window->GetSurface(), &formatCount, t_Details.formats.data());
	}

	//Get Present Data
	uint32_t t_PresentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(a_Device, rm_Window->GetSurface(), &t_PresentModeCount, nullptr);

	if (t_PresentModeCount != 0)
	{
		t_Details.presentModes.resize(t_PresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(a_Device, rm_Window->GetSurface(), &t_PresentModeCount, t_Details.presentModes.data());
	}

	return t_Details;
}

#pragma region Private Functions

bool Context::IsDeviceSuitable(VkPhysicalDevice a_Device)
{
	//Queues are supported.
	QueueFamilyIndices t_Indices = FindQueueFamilies(a_Device);

	//Extentions from m_DeviceExtensions are supported.
	bool t_ExtensionsSupported = CheckDeviceExtensionSupport(a_Device);

	//Swapchain is sufficiently supported.
	bool t_SwapChainAdequate = false;
	if (t_ExtensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(a_Device);
		t_SwapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures t_SupportedFeatures;
	vkGetPhysicalDeviceFeatures(a_Device, &t_SupportedFeatures);

	//The device works with everything that the program needs.
	return t_Indices.isComplete() && t_ExtensionsSupported && t_SwapChainAdequate && t_SupportedFeatures.samplerAnisotropy;;
}

bool Context::CheckDeviceExtensionSupport(VkPhysicalDevice a_Device)
{
	uint32_t t_ExtensionCount;
	vkEnumerateDeviceExtensionProperties(a_Device, nullptr, &t_ExtensionCount, nullptr);

	std::vector<VkExtensionProperties> t_AvailableExtensions(t_ExtensionCount);
	vkEnumerateDeviceExtensionProperties(a_Device, nullptr, &t_ExtensionCount, t_AvailableExtensions.data());

	std::set<std::string> t_RequiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	for (const auto& extension : t_AvailableExtensions)
	{
		t_RequiredExtensions.erase(extension.extensionName);
	}

	return t_RequiredExtensions.empty();
}
#pragma endregion
