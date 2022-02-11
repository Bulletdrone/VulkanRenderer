#pragma once
#include <vector>
#include <unordered_map>
#include <string>

#pragma warning (push, 0)
#include <glm/vec2.hpp>
#include <glfw3.h>
#include <Vulkan/vulkan.hpp>
#pragma warning (pop)
#include "GUITypes.h"
#include "GUIHandle.h"

typedef int ImGuiWindowFlags;

namespace GUI 
{
	struct GUICreationData
	{
		glm::vec2 position;
		glm::vec2 scale;

		const char* windowName;

		//true by default
		bool resizable = true;
	};

	class GUIWindow
	{
	public:
		GUIWindow() {};
		GUIWindow(GUICreationData a_CreationData);
		~GUIWindow();

		//Returns true if a condition changed that requires the values to be send back.
		bool Update(bool a_IsChild);

		void SetActive(bool a_IsActive) { m_Active = a_IsActive; }

		void AddElement(GUITypes::GUIElement* a_Element);

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

		void Init(const VkDevice a_Device, const VkInstance a_Instance, const VkPhysicalDevice a_PhysDevice, const VkQueue a_Queue, const VkRenderPass a_MainRenderPass);
		void Update();

		//Creates an GUIWindow and returns a pointer of the one being creatd inside m_GUIWindows.
		GUIHandle CreateGUIWindow(GUICreationData a_CreationData);
		bool DeleteGUIWindow(GUIHandle a_Handle);

		template <typename TGUIElement>
		bool AddElementToGUIWindow(GUIHandle a_Handle, TGUIElement a_Element);

	private:
		std::unordered_map<uint32_t, GUIWindow*> m_GUIWindows;

		uint32_t m_HandlerCount = 0;

		VkDescriptorPool m_ImguiPool = VK_NULL_HANDLE;

		VkDevice m_Device = VK_NULL_HANDLE;
		GLFWwindow* p_Window = nullptr;
	};

	template<typename TGUIElement>
	inline bool GUISystem::AddElementToGUIWindow(GUIHandle a_Handle, TGUIElement a_Element)
	{
		auto it = m_GUIWindows.find(a_Handle);

		if (it != m_GUIWindows.end() && a_Handle.IsValid())
		{
			GUITypes::GUIElement* element = new TGUIElement(a_Element);
			it->second->AddElement(element);

			return true;
		}

		printf("GUI: Failed to add element. The handler isn't valid.");
		return false;
	}
}