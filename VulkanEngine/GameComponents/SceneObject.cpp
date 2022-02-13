#include "Precomp.h"
#include "SceneObject.h"

namespace Engine
{
	SceneObject::SceneObject(
		Space a_WorldSpace,
		Transform* a_Parent,
		GUIHandle a_GUIHandle,
		MeshHandle a_MeshHandle,
		MaterialHandle a_MaterialHandle,
		glm::vec3 a_Pos,
		glm::vec3 a_Rot,
		glm::vec3 a_Scale)
	{
		m_Transform.SetParent(a_Parent);

		m_GUIHandle = a_GUIHandle;
		m_MeshHandle = a_MeshHandle;
		m_MaterialHandle = a_MaterialHandle;

		m_Transform.SetPosition(a_Pos, a_WorldSpace);
		m_Transform.SetRotation(glm::quat(a_Rot), a_WorldSpace);
		m_Transform.SetScale(a_Scale, a_WorldSpace);
	}

	SceneObject::SceneObject(const SceneObject& a_Rhs)
	{
		m_GUIHandle = a_Rhs.GetWindowHandle();
		m_MaterialHandle = a_Rhs.GetMaterialHandle();
		m_MeshHandle = a_Rhs.GetMeshHandle();
	}

	SceneObject::~SceneObject()
	{

	}
}