#pragma once
#include <cstdint>
constexpr uint32_t GUI_NULL_HANDLE = 0;

struct GUIHandle
{
	GUIHandle() {};
	GUIHandle(const uint32_t a_Handle) { m_Handle = a_Handle; } 

	operator uint32_t() const
	{
		return m_Handle;
	};

	void operator=(const uint32_t a_Rhs)
	{
		m_Handle = a_Rhs;
	}

	bool IsValid() const
	{
		return m_Handle != GUI_NULL_HANDLE;
	}

private:
	uint32_t m_Handle = GUI_NULL_HANDLE;
};