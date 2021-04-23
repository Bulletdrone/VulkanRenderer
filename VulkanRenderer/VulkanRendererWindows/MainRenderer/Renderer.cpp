#define VK_USE_PLATFORM_WIN32_KHR
#include "Renderer.h"

#include <iostream>
#include <set>
#include <cstdint>
#include <algorithm>


Renderer::Renderer()
{
	//Setting up GLFW.
	m_Window = new Window(800, 600, "VulkanTest");

	if (DE_EnableValidationLayers)
		DE_VKDebug = new VulkanDebug(m_VKInstance);

	CreateVKInstance();
	
	if (DE_EnableValidationLayers)
		DE_VKDebug->SetupDebugMessanger(nullptr);

	m_Window->SetupVKWindow(m_VKInstance);
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
}

Renderer::~Renderer()
{
	for (auto imageView : m_VKSwapChainImageViews) {
		vkDestroyImageView(m_VKDevice, imageView, nullptr);
	}

	if (DE_EnableValidationLayers)
		delete DE_VKDebug;

	vkDestroySwapchainKHR(m_VKDevice, m_VKSwapChain, nullptr);
	vkDestroyDevice(m_VKDevice, nullptr);

	vkDestroySurfaceKHR(m_VKInstance, m_Window->GetSurface(), nullptr);
	vkDestroyInstance(m_VKInstance, nullptr);
	delete m_Window;
}

void Renderer::CreateVKInstance()
{
	//If debug is enabled check if the layers are supported.
	if (DE_EnableValidationLayers && !DE_VKDebug->CheckValidationLayerSupport())
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

	auto t_Extentions = m_Window->GetRequiredExtentions(DE_EnableValidationLayers);

	t_CreateInfo.enabledExtensionCount = static_cast<uint32_t>(t_Extentions.size());
	t_CreateInfo.ppEnabledExtensionNames = t_Extentions.data();

	//Set the Debug Layers if debug is enabled, otherwise set it to 0
	if (DE_EnableValidationLayers)
	{
		DE_VKDebug->AddDebugLayerInstance(t_CreateInfo);

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		DE_VKDebug->PopulateDebugStartupMessengerCreateInfo(debugCreateInfo);
		t_CreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else
	{
		t_CreateInfo.enabledLayerCount = 0;
		t_CreateInfo.pNext = nullptr;
	}

	//Create and check if the Instance is a success.
	if (vkCreateInstance(&t_CreateInfo, nullptr, &m_VKInstance) != VK_SUCCESS)
	{
		throw printf("Vulkan not successfully Instanced.");
	}
}

void Renderer::PickPhysicalDevice()
{
	uint32_t t_DeviceCount = 0;
	vkEnumeratePhysicalDevices(m_VKInstance, &t_DeviceCount, nullptr);

	//No devices that support Vulcan.
	if (t_DeviceCount == 0) 
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> t_Devices(t_DeviceCount);
	vkEnumeratePhysicalDevices(m_VKInstance, &t_DeviceCount, t_Devices.data());

	for (const VkPhysicalDevice& device : t_Devices) 
	{
		if (IsDeviceSuitable(device)) 
		{
			m_VKPhysicalDevice = device;
			break;
		}
	}

	if (m_VKPhysicalDevice == VK_NULL_HANDLE) 
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}

}

void Renderer::CreateLogicalDevice()
{
	QueueFamilyIndices t_Indices = FindQueueFamilies(m_VKPhysicalDevice);

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

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo t_CreateInfo{};
	t_CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	t_CreateInfo.queueCreateInfoCount = static_cast<uint32_t>(t_QueueCreateInfos.size());
	t_CreateInfo.pQueueCreateInfos = t_QueueCreateInfos.data();

	t_CreateInfo.pEnabledFeatures = &deviceFeatures;

	t_CreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	t_CreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

	if (DE_EnableValidationLayers) 
	{
		DE_VKDebug->AddDebugLayerInstanceDevice(t_CreateInfo);
	}
	else 
	{
		t_CreateInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_VKPhysicalDevice, &t_CreateInfo, nullptr, &m_VKDevice) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(m_VKDevice, t_Indices.graphicsFamily.value(), 0, &m_VKGraphicsQueue);
	vkGetDeviceQueue(m_VKDevice, t_Indices.presentFamily.value(), 0, &m_VKPresentQueue);
}

void Renderer::CreateSwapChain()
{
	SwapChainSupportDetails t_SwapChainSupport = QuerySwapChainSupport(m_VKPhysicalDevice);

	//Getting the needed values from the PhysicalDevice.
	VkSurfaceFormatKHR t_SurfaceFormat = ChooseSwapSurfaceFormat(t_SwapChainSupport.formats);
	VkPresentModeKHR t_PresentMode = ChooseSwapPresentMode(t_SwapChainSupport.presentModes);
	VkExtent2D t_Extent = ChooseSwapExtent(t_SwapChainSupport.capabilities);

	//Setting up the SwapChain.
	uint32_t t_ImageCount = t_SwapChainSupport.capabilities.minImageCount + 1;

	if (t_SwapChainSupport.capabilities.maxImageCount > 0 && t_ImageCount > t_SwapChainSupport.capabilities.maxImageCount) 
	{
		t_ImageCount = t_SwapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR t_CreateInfo{};
	t_CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	t_CreateInfo.surface = m_Window->GetSurface();

	t_CreateInfo.minImageCount = t_ImageCount;
	t_CreateInfo.imageFormat = t_SurfaceFormat.format;
	t_CreateInfo.imageColorSpace = t_SurfaceFormat.colorSpace;
	t_CreateInfo.imageExtent = t_Extent;
	t_CreateInfo.imageArrayLayers = 1;
	t_CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices t_Indices = FindQueueFamilies(m_VKPhysicalDevice);
	uint32_t t_QueueFamilyIndices[] = { t_Indices.graphicsFamily.value(), t_Indices.presentFamily.value() };

	if (t_Indices.graphicsFamily != t_Indices.presentFamily) 
	{
		t_CreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		t_CreateInfo.queueFamilyIndexCount = 2;
		t_CreateInfo.pQueueFamilyIndices = t_QueueFamilyIndices;
	}
	else 
	{
		t_CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		t_CreateInfo.queueFamilyIndexCount = 0; // Optional
		t_CreateInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	//Starting Transform for Objects such as a specific starting rotation, now just the standard 0 for all.
	t_CreateInfo.preTransform = t_SwapChainSupport.capabilities.currentTransform;

	//No Transparent Window.
	t_CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	t_CreateInfo.presentMode = t_PresentMode;
	t_CreateInfo.clipped = VK_TRUE;

	//If a new SwapChain gets created such as resizing the window, ignore for now.
	t_CreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_VKDevice, &t_CreateInfo, nullptr, &m_VKSwapChain) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(m_VKDevice, m_VKSwapChain, &t_ImageCount, nullptr);
	m_VKSwapChainImages.resize(t_ImageCount);
	vkGetSwapchainImagesKHR(m_VKDevice, m_VKSwapChain, &t_ImageCount, m_VKSwapChainImages.data());

	//Setting the Local variables
	m_VKSwapChainImageFormat = t_SurfaceFormat.format;
	m_VKSwapChainExtent = t_Extent;
}

void Renderer::CreateImageViews()
{
	m_VKSwapChainImageViews.resize(m_VKSwapChainImages.size());

	for (size_t i = 0; i < m_VKSwapChainImages.size(); i++)
	{
		VkImageViewCreateInfo t_CreateInfo{};
		t_CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		t_CreateInfo.image = m_VKSwapChainImages[i];
		t_CreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		t_CreateInfo.format = m_VKSwapChainImageFormat;

		//The standard color values.
		t_CreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		t_CreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		t_CreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		t_CreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		//What is the Image's Purpose
		t_CreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		t_CreateInfo.subresourceRange.baseMipLevel = 0;
		t_CreateInfo.subresourceRange.levelCount = 1;
		t_CreateInfo.subresourceRange.baseArrayLayer = 0;
		t_CreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_VKDevice, &t_CreateInfo, nullptr, &m_VKSwapChainImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}
	}
}

QueueFamilyIndices Renderer::FindQueueFamilies(VkPhysicalDevice a_Device)
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
			vkGetPhysicalDeviceSurfaceSupportKHR(a_Device, i, m_Window->GetSurface(), &t_PresentSupport);

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

SwapChainSupportDetails Renderer::QuerySwapChainSupport(VkPhysicalDevice a_Device) {
	SwapChainSupportDetails t_Details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(a_Device, m_Window->GetSurface(), &t_Details.capabilities);

	//Get Format Data
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(a_Device, m_Window->GetSurface(), &formatCount, nullptr);

	if (formatCount != 0) 
	{
		t_Details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(a_Device, m_Window->GetSurface(), &formatCount, t_Details.formats.data());
	}

	//Get Present Data
	uint32_t t_PresentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(a_Device, m_Window->GetSurface(), &t_PresentModeCount, nullptr);

	if (t_PresentModeCount != 0) 
	{
		t_Details.presentModes.resize(t_PresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(a_Device, m_Window->GetSurface(), &t_PresentModeCount, t_Details.presentModes.data());
	}

	return t_Details;
}

VkSurfaceFormatKHR Renderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& a_AvailableFormats) 
{
    for (const auto& t_AvailableFormat : a_AvailableFormats)
	{
        if (t_AvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && t_AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return t_AvailableFormat;
        }
    }

    return a_AvailableFormats[0];
}

VkPresentModeKHR Renderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& a_AvailablePresentModes)
{
	for (const auto& t_AvailablePresentMode : a_AvailablePresentModes) 
	{
		if (t_AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return t_AvailablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& a_Capabilities)
{
	if (a_Capabilities.currentExtent.width != UINT32_MAX) {
		return a_Capabilities.currentExtent;
	}
	else {
		int t_Width, t_Height;
		glfwGetFramebufferSize(m_Window->GetWindow(), &t_Width, &t_Height);

		VkExtent2D t_ActualExtent = {
			static_cast<uint32_t>(t_Width),
			static_cast<uint32_t>(t_Height)
		};

		t_ActualExtent.width = std::clamp(t_ActualExtent.width, a_Capabilities.minImageExtent.width, a_Capabilities.maxImageExtent.width);
		t_ActualExtent.height = std::clamp(t_ActualExtent.height, a_Capabilities.minImageExtent.height, a_Capabilities.maxImageExtent.height);
		return t_ActualExtent;
	}
}
	
bool Renderer::IsDeviceSuitable(VkPhysicalDevice a_Device)
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

	//The device works with everything that the program needs.
	return t_Indices.isComplete() && t_ExtensionsSupported && t_SwapChainAdequate;
}

bool Renderer::CheckDeviceExtensionSupport(VkPhysicalDevice a_Device)
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