#pragma once

#include "MeshData.h"
#include <Structs/Texture.h>

//Pre-build renderdata that is created on startup. 
//This will include loaded files and basic shapes.
//You can assign some parts yourself like a texture or color depending what you want.
//RenderObjectData handles it's own deletion, so all the parts here shouldn't be deleted.
struct RenderObjectData
{
	RenderObjectData(MeshData* a_MeshData) 
		: m_MeshData(a_MeshData) {}

	RenderObjectData(MeshData* p_MeshData, glm::vec4 a_Color)
		: m_MeshData(p_MeshData), m_Color(a_Color) {}

	~RenderObjectData() { delete m_MeshData; };

	MeshData* m_MeshData;

	glm::vec4 m_Color = glm::vec4(1);
};