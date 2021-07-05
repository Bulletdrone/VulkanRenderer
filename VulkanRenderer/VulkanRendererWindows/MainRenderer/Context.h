#pragma once
#include <VulkanDebugger/VulkanDebug.h>
#include <Structs/ContextData.h>

#include <Window.h>

#include <optional>

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

//Special struct for the Family Queue.
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

class Context
{
public:
	Context(bool a_bDebugEnabled, Window* p_Window);
	~Context();
	void CleanupContext();

public:
	void CreateVKInstance(const ContextData& r_ContextData);
	void PickPhysicalDevice();
	void CreateLogicalDevice(VkQueue& vkr_GraphicsQueue, VkQueue& vkr_PresentQueue);

public:
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice a_Device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice a_Device);

private:
	bool IsDeviceSuitable(VkPhysicalDevice a_Device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice a_Device);

public:
	VkInstance mvk_Instance;
	VkPhysicalDevice mvk_PhysicalDevice = VK_NULL_HANDLE;
	VkDevice mvk_Device;

private:
	Window* rm_Window;

	//Constant data.
	const std::vector<const char*> m_DeviceExtensions = {
VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	//Debug
	bool m_bDebugEnabled;
	VulkanDebug* DE_VulkanDebug = nullptr;
};

