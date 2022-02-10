#pragma once
#include <vector>
#include <string>

#pragma warning (push, 0)
#include <glm/vec2.hpp>
#include <glfw3.h>
#include <Vulkan/vulkan.hpp>
#pragma warning (pop)
#include "GUITypes.h"

typedef int ImGuiWindowFlags;

class GUIWindow
{
public:
	GUIWindow();
	~GUIWindow();

	void Init(glm::vec2 a_Position, glm::vec2 a_Scale, const char* a_WindowName, bool a_Resizable = true);
	//Returns true if a condition changed that requires the values to be send back.
	bool Update(bool a_IsChild);

	void SetActive(bool a_IsActive) { m_Active = a_IsActive; }

	template <typename TGUIElement>
	void AddElement(TGUIElement a_Element);

	GUIWindow& CreateChildWindow();

private:
	glm::vec2 m_Position = glm::vec2(0, 0);
	glm::vec2 m_Scale = glm::vec2(100, 100);
	ImGuiWindowFlags m_WindowFlags = 0;

	bool m_Active = true;

	const char* m_WindowName = "Default";

	std::vector<GUITypes::GUIElement*> m_GUIElements;

	std::vector<GUIWindow> m_ChildWindows;
};

template <typename TGUIElement>
void GUIWindow::AddElement(TGUIElement a_Element)
{
	GUITypes::GUIElement* element = new TGUIElement(a_Element);
	m_GUIElements.push_back(element);
}



class GUISystem
{
public:
	GUISystem(GLFWwindow* a_Window);
	~GUISystem();

	void Init(VkCommandBuffer& r_CommandBuffer, const VkDevice a_Device, const VkInstance a_Instance, const VkPhysicalDevice a_PhysDevice, const VkQueue a_Queue, const VkRenderPass a_MainRenderPass);
	void Update();

	//Creates an GUIWindow and returns a pointer of the one being creatd inside m_GUIWindows.
	GUIWindow* CreateGUIWindow();

private:
	std::vector<GUIWindow> m_GUIWindows;

	VkDescriptorPool m_ImguiPool = VK_NULL_HANDLE;
	
	VkDevice m_Device = VK_NULL_HANDLE;
	GLFWwindow* p_Window = nullptr;
};