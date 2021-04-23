#include "Window.h"

#include <iostream>

Window::Window(int a_Width, int a_Height, const char* a_WindowName)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_Window = glfwCreateWindow(a_Width, a_Height, a_WindowName, nullptr, nullptr);
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);

    glfwTerminate();
}

void Window::SetupVKWindow(const VkInstance& rm_VKInstance)
{
    VkWin32SurfaceCreateInfoKHR t_CreateInfo{};
    t_CreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    t_CreateInfo.hwnd = glfwGetWin32Window(m_Window);
    t_CreateInfo.hinstance = GetModuleHandle(nullptr);

    if (glfwCreateWindowSurface(rm_VKInstance, m_Window, nullptr, &m_Surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

std::vector<const char*> Window::GetRequiredExtentions(const bool a_EnabledValidationLayers)
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (a_EnabledValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}
