#pragma once
#include <vector>
#include <unordered_map>
#include <string>

#pragma warning (push, 0)
#include <glfw3.h>
#include <Vulkan/vulkan.hpp>
#pragma warning (pop)

#include "GUIWindow.h"

namespace GUI 
{
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
		const char* GetGUIWindowName(GUIHandle a_Handle);

		template <typename TGUIElement>
		TGUIElement* AddElementToGUIWindow(GUIHandle a_Handle, TGUIElement a_Element);

	private:
		std::unordered_map<uint32_t, GUIWindow*> m_GUIWindows;

		uint32_t m_HandlerCount = 0;

		VkDescriptorPool m_ImguiPool = VK_NULL_HANDLE;

		VkDevice m_Device = VK_NULL_HANDLE;
		GLFWwindow* p_Window = nullptr;
	};

	template<typename TGUIElement>
	inline TGUIElement* GUISystem::AddElementToGUIWindow(GUIHandle a_Handle, TGUIElement a_Element)
	{
		auto it = m_GUIWindows.find(a_Handle);

		if (it != m_GUIWindows.end() && a_Handle.IsValid())
		{
			GUITypes::GUIElement* element = new TGUIElement(a_Element);
			it->second->AddElement(element);

			return dynamic_cast<TGUIElement*>(element);
		}

		printf("GUI: Failed to add element. The handler isn't valid.");
		return nullptr;
	}
}