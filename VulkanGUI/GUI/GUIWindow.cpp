#include "pch.h"
#include "GUIWindow.h"

#pragma warning (push, 0)
#include "ImGUI/imgui.h"
#pragma warning (pop)

namespace GUI
{
	GUIWindow::GUIWindow(GUICreationData a_CreationData)
	{
		m_Position = a_CreationData.position;
		m_Scale = a_CreationData.scale;

		m_WindowName = a_CreationData.windowName;

		m_Parent = a_CreationData.parent;
		m_ParentName = a_CreationData.parentname;

		if (!a_CreationData.resizable)
			m_WindowFlags = ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse;
	}

	GUIWindow::~GUIWindow()
	{
		for (GUITypes::GUIElement* it : m_GUIElements)
		{
			delete it;
		}
		m_GUIElements.clear();
	}

	void GUIWindow::Update()
	{
		//if the window is inactive just return.
		if (!m_Active) return;

#ifdef _DEBUG
		if ((m_Parent != GUI_NULL_HANDLE && m_ParentName == nullptr) || 
			(m_Parent == GUI_NULL_HANDLE && m_ParentName != nullptr))
		{
			printf("GUIWindow has a name/parent but no name/parent. Might result in undefined behaviour.");
		}
#endif // _DEBUG

		if (m_Parent != GUI_NULL_HANDLE)
		{
			//setup for parent.
			ImGui::Begin(m_ParentName, NULL, m_WindowFlags);
			ImGui::PushID(handle);
			if (!ImGui::CollapsingHeader(m_WindowName))
			{
				ImGui::PopID();
				ImGui::End();
				return;
			}
		}
		else
		{
			ImGui::SetNextWindowPos(ImVec2(m_Position.x, m_Position.y), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(m_Scale.x, m_Scale.y), ImGuiCond_Once);

			ImGui::Begin(m_WindowName, NULL, m_WindowFlags);
		}

		for (GUITypes::GUIElement* it : m_GUIElements)
		{
			it->DrawElement();
		}

		

		if (m_Parent != GUI_NULL_HANDLE)
			ImGui::PopID();
	
		ImGui::End();
	}

	void GUIWindow::AddElement(GUITypes::GUIElement* a_Element)
	{
		m_GUIElements.push_back(a_Element);
	}
}