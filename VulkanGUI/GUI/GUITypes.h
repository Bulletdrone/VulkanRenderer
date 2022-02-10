#pragma once
#include <string>

namespace GUI
{
	namespace GUITypes
	{
		struct GUIElement
		{
			virtual ~GUIElement() {};

			const char* name = "Default";

			virtual bool DrawElement() = 0;
		};

		struct Text : public GUIElement
		{
			bool DrawElement() override;

			std::string* value = nullptr;
		};

		struct StaticText : public GUIElement
		{
			bool DrawElement() override;

			const char* value = nullptr;
		};

		struct Button : public GUIElement
		{
			bool DrawElement() override;

			operator bool* () const { return value; }

			bool* value = nullptr;
		};

		struct ColorEdit : public GUIElement
		{
			bool DrawElement() override;

			float* value = nullptr;
		};

#pragma region Sliders

		//imGUI slider using vec2.
		struct ISlider : public GUIElement
		{
			bool DrawElement() override;

			int min = 0;
			int max = 0;

			int elementSize = 0;
			int* value = nullptr;
		};

		//imGUI slider using vec2.
		struct FSlider : public GUIElement
		{
			bool DrawElement() override;

			float min = 0;
			float max = 0;

			int elementSize = 0;
			float* value = nullptr;
		};

#pragma endregion
	}
}