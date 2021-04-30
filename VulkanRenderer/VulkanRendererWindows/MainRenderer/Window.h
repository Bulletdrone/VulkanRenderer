#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

#include <vector>

class Window
{
public:
	Window(int a_Width, int a_Height, const char* a_WindowName);
	~Window();

	void SetupVKWindow(const VkInstance& rm_VKInstance);
	void CheckMinimized();

	std::vector<const char*> GetRequiredExtentions(const bool a_EnabledValidationLayers);

	//Getters
	GLFWwindow* GetWindow() const { return m_Window; }
	VkSurfaceKHR& GetSurface() { return m_Surface; }

	bool m_FrameBufferResized = false;
private:
	//GLFW Window Data
	GLFWwindow* m_Window;
	VkSurfaceKHR m_Surface;
};

static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) 
{
	auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	app->m_FrameBufferResized = true;
}
