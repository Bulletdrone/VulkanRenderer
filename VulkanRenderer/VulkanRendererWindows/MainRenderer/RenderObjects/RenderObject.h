#pragma once
#include "Tools/RenderHandle.h"
#include "../../../../VulkanEngine/GameComponents/Transform.h"

struct RenderObject
{
	Engine::Transform* p_Transform = nullptr;

	MeshHandle meshHandle = RENDER_NULL_HANDLE;
	MaterialHandle materialHandle = RENDER_NULL_HANDLE;

	bool IsValid()
	{
		bool t_Valid = true;
		
		if (p_Transform == nullptr)
		{
			printf("No transform on renderobject.");
			t_Valid = false;
		}
		if (meshHandle == RENDER_NULL_HANDLE)
		{
			printf("No meshHandle on renderobject.");
			t_Valid = false;
		}
		if (materialHandle == RENDER_NULL_HANDLE)
		{
			printf("No materialHandle on renderobject.");
			t_Valid = false;
		}

		return t_Valid;
	}
};