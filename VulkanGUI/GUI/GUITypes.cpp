#include "GUITypes.h"

#include "Logger/Logger.h"

#pragma warning (push, 0)
#include "ImGUI/imgui.h"
#include <commdlg.h>
#pragma warning (pop)

constexpr uint16_t MAXFILE = 256;

namespace GUI
{
	namespace GUITypes
	{
		bool Text::DrawElement()
		{
			ImGui::TextWrapped(value->c_str());
			return false;
		}

		bool StaticText::DrawElement()
		{
			ImGui::TextWrapped(value);
			return false;
		}

		bool Button::DrawElement()
		{
			return ImGui::Button(name);
		}

		bool GetPathButton::DrawElement()
		{
			if (ImGui::Button(name))
			{
				OPENFILENAME t_Ofn;
				TCHAR t_ReadFile[MAXFILE]{0};

				ZeroMemory(&t_Ofn, sizeof(OPENFILENAME));
				t_Ofn.lStructSize = sizeof(OPENFILENAME);
				t_Ofn.lpstrFile = t_ReadFile;
				t_Ofn.nMaxFile = MAXFILE;
				t_Ofn.lpstrFilter = fileFilter;
				t_Ofn.nFilterIndex = 1;
				t_Ofn.lpstrFileTitle = nullptr;
				t_Ofn.nMaxFileTitle = 0;
				t_Ofn.lpstrInitialDir = nullptr;
				t_Ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				if (GetOpenFileName(&t_Ofn) == TRUE)
				{
					if (filePath != nullptr)
						delete[] filePath;

					filePath = new char[wcslen(t_Ofn.lpstrFile) + 1];
					wsprintfA(filePath, "%S", t_Ofn.lpstrFile);
					active = true;
					return true;
				}
			}
			active = false;
			return false;
		}

		bool ColorEdit::DrawElement()
		{
			return ImGui::ColorEdit4(name, value, ImGuiColorEditFlags_Float);
		}

		bool ISlider::DrawElement()
		{
			switch (elementSize)
			{
			case 1:
				return ImGui::SliderInt(name, value, min, max);
				break;
			case 2:
				return ImGui::SliderInt2(name, value, min, max);
				break;
			case 3:
				return ImGui::SliderInt3(name, value, min, max);
				break;
			case 4:
				return ImGui::SliderInt4(name, value, min, max);
				break;
			default:
				Logger::Assert((elementSize < 4 || elementSize > 1), "GUIType ISlider elementType lower then 1 or higher then 4.");
				break;
			}
			return false;
		}

		bool ITextSlider::DrawElement()
		{
			if (texts.size() == 0)
			{
				return false;
			}
			return ImGui::SliderInt(texts[currentValue], &currentValue, 0, static_cast<int>(texts.size() - 1));
		}

		bool FSlider::DrawElement()
		{
			switch (elementSize)
			{
			case 1:
				return ImGui::SliderFloat(name, value, min, max);
				break;
			case 2:
				return ImGui::SliderFloat2(name, value, min, max);
				break;
			case 3:
				return ImGui::SliderFloat3(name, value, min, max);
				break;
			case 4:
				return ImGui::SliderFloat4(name, value, min, max);
				break;
			default:
				Logger::Assert((elementSize < 4 || elementSize > 1), "GUIType FSlider elementType lower then 1 or higher then 4.");
				break;
			}
			return false;
		}
		bool InputF::DrawElement()
		{
			switch (elementSize)
			{
			case 1:
				return ImGui::InputFloat(name, value);
				break;
			case 2:
				return ImGui::InputFloat2(name, value);
				break;
			case 3:
				return ImGui::InputFloat3(name, value);
				break;
			case 4:
				return ImGui::InputFloat4(name, value);
				break;
			default:
				Logger::Assert((elementSize < 4 || elementSize > 1), "GUIType InputF elementType lower then 1 or higher then 4.");
				break;
			}
			return false;
		}
}
}