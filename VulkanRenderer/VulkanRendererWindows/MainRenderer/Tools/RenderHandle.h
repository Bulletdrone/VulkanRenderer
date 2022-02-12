#pragma once
#include <stdint.h>

constexpr uint32_t RENDER_NULL_HANDLE = UINT32_MAX;

typedef struct BasicRenderHandle MeshHandle;
typedef struct BasicRenderHandle MaterialHandle;

struct BasicRenderHandle
{
	BasicRenderHandle() {};
	BasicRenderHandle(const uint32_t a_Handle) { m_Handle = a_Handle; }

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
		return m_Handle != RENDER_NULL_HANDLE;
	}

private:
	uint32_t m_Handle = RENDER_NULL_HANDLE;
};