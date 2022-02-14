#pragma once
#include "GUITypes.h"
#include "GUIHandle.h"

#pragma warning (push, 0)
#include <glm/vec2.hpp>
#pragma warning (pop)

typedef int ImGuiWindowFlags;

namespace GUI
{
	struct GUICreationData
	{
		glm::vec2 position;
		glm::vec2 scale;

		const char* windowName = "unnamed Window";

		//GUI_NULL_HANDLE by default.
		GUIHandle parent = GUI_NULL_HANDLE;
		//Must be filled in if parent is filled in.
		const char* parentname = nullptr;

		//true by default
		bool resizable = true;
	};

	class GUIWindow
	{
	public:
		GUIWindow() {};
		GUIWindow(GUICreationData a_CreationData);
		~GUIWindow();

		void Update();

		void SetActive(bool a_IsActive) { m_Active = a_IsActive; }

		void AddElement(GUITypes::GUIElement* a_Element);

		const bool IsChild() const { return m_Parent != GUI_NULL_HANDLE; }
		const char* GetName() const { return m_WindowName; }

		GUIHandle handle = GUI_NULL_HANDLE;

	private:
		//Parent Variables
		GUIHandle m_Parent = GUI_NULL_HANDLE;
		const char* m_ParentName = nullptr;

		//Window CreationData
		glm::vec2 m_Position = glm::vec2(0, 0);
		glm::vec2 m_Scale = glm::vec2(100, 100);
		ImGuiWindowFlags m_WindowFlags = 0;

		bool m_Active = true;

		const char* m_WindowName = nullptr;

		std::vector<GUITypes::GUIElement*> m_GUIElements;
	};
}