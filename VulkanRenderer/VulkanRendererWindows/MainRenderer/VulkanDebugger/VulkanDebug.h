#pragma once


#pragma warning (push, 0)
#include <Vulkan/vulkan.h>
#pragma warning (pop)
#include <vector>

class VulkanDebug
{
public:
	VulkanDebug(const VkInstance& r_VKInstance);
	~VulkanDebug();

	void PopulateDebugStartupMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void SetupStartupDebugMessanger();


	void SetupDebugMessanger(const VkAllocationCallbacks* r_AllocCallback);

	bool CheckValidationLayerSupport();
	void AddDebugLayerInstance(VkInstanceCreateInfo& a_CreateInfo);
	void AddDebugLayerInstanceDevice(VkDeviceCreateInfo& r_CreateInfo);

	//Getters
	std::vector<const char*> GetValidationLayers() const { return m_ValidationLayers; }

private:
	const VkInstance& rm_VKInstance;
	VkDebugUtilsMessengerEXT m_DebugMessanger = VK_NULL_HANDLE;

	//Vulcan ValidationData.
	std::vector<const char*> m_ValidationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

};

