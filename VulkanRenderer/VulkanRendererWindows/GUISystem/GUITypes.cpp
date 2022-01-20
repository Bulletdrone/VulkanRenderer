#include "GUITypes.h"
#include <assert.h>

#pragma warning (push, 0)
#include "ImGUI/imgui.h"
#pragma warning (pop)


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
		if (ImGui::Button(name))
		{
			*value = !*value;
			return true;
		}

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
			assert(elementSize < 4 || elementSize > 1);
			break;
		}
		return false;
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
			assert(elementSize < 4 || elementSize > 1);
			break;
		}
		return false;
	}
}
