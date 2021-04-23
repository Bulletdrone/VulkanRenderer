#include "VulkanDebug.h"

#include <iostream>
#include <cstring>

#pragma region Finding Functions related to Debugging.
//Create the actual function for setting up the debugger.
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}
#pragma endregion

//Debug Message Caller.
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

//Vulkan Debugger.
VulkanDebug::VulkanDebug(const VkInstance& r_VKInstance)
    :   rm_VKInstance(r_VKInstance)
{}

VulkanDebug::~VulkanDebug()
{
    DestroyDebugUtilsMessengerEXT(rm_VKInstance, m_DebugMessanger, nullptr);
}

void VulkanDebug::PopulateDebugStartupMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void VulkanDebug::SetupStartupDebugMessanger() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugStartupMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(rm_VKInstance, &createInfo, nullptr, &m_DebugMessanger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void VulkanDebug::SetupDebugMessanger(const VkAllocationCallbacks* r_AllocCallback)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional

    if (CreateDebugUtilsMessengerEXT(rm_VKInstance, &createInfo, r_AllocCallback, &m_DebugMessanger) != VK_SUCCESS)
        throw std::runtime_error("failed to set up debug messenger!");
}

bool VulkanDebug::CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : m_ValidationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void VulkanDebug::AddDebugLayerInstance(VkInstanceCreateInfo& r_CreateInfo)
{
    r_CreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
    r_CreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
}

void VulkanDebug::AddDebugLayerInstanceDevice(VkDeviceCreateInfo& r_CreateInfo)
{
    r_CreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
    r_CreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
}