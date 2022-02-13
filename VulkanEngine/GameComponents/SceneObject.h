#pragma once

#include "Transform.h"
#include <Tools/RenderHandle.h>
#include <GUI/GUIHandle.h>

namespace Engine
{
	class SceneObject
	{
	public:
		SceneObject(
			Space a_WorldSpace,
			Transform* a_Parent = nullptr,
			GUIHandle a_GUIHandle = GUI_NULL_HANDLE,
			MeshHandle a_MeshHandle = RENDER_NULL_HANDLE,
			MaterialHandle a_MaterialHandle = RENDER_NULL_HANDLE,
			glm::vec3 a_Pos = glm::vec3(0),
			glm::vec3 a_Rot = glm::vec3(0),
			glm::vec3 a_Scale = glm::vec3(1));

		SceneObject(const SceneObject& a_Rhs);
		~SceneObject();




		Transform& GetTransform() { return m_Transform; }

		const GUIHandle GetWindowHandle() const { return m_GUIHandle; }
		const MeshHandle GetMeshHandle() const { return m_MeshHandle; }
		const MaterialHandle GetMaterialHandle() const { return m_MaterialHandle; }

	private:
		Transform m_Transform;

		GUIHandle m_GUIHandle = GUI_NULL_HANDLE;
		MaterialHandle m_MaterialHandle = RENDER_NULL_HANDLE;
		MeshHandle m_MeshHandle = RENDER_NULL_HANDLE;
	};
}