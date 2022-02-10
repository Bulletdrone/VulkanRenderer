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

	/*	Add text on this window.
		@param a_Text, Pointer to the text.*/
	void AddText(std::string* a_Text);

	/*	Add text on this window, this one isn't meant to be changed.
		@param a_Text, the text, this is non-pointer and static.*/
	void AddText(const char* a_Text);

	/*	Create an slider for floats, can be to a maximum of 4 floats.
		@param a_Name, slider name.
		@param a_Values, the value_ptr of a glm::vec4. Must be that.*/
	void AddColorEdit(const char* a_Name, float* a_Values);

	/*	Create an slider for intergers, can be to a maximum of 4 ints.
		@param a_Name, slider name.
		@param a_Values, the int as a pointer. (max 4 ints)
		@param a_ElementSize, how big the int array is.
		@param a_Max, the maximum value for the slider.
		@param a_Min, the minimum value for the slider. */
	void AddIntSlider(const char* a_Name, int* a_Values, int a_ElementSize, int a_Min, int a_Max);

	/*	Create an slider for floats, can be to a maximum of 4 floats.
		@param a_Name, slider name.
		@param a_Values, the float as a pointer. (max 4 floats)
		@param a_ElementSize, how big the float array is.
		@param a_Max, the maximum value for the slider.
		@param a_Min, the minimum value for the slider. */
	void AddFloatSlider(const char* a_Name, float* a_Values, int a_ElementSize, float a_Min, float a_Max);


	/*	Create an imGUI button on this window.
		@param a_Name, button name.
		@param a_Values, Pointer to the bool.*/
	void AddButton(const char* a_Name, bool* a_Values);

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
	
	VkDevice m_Device;
	GLFWwindow* p_Window;
};