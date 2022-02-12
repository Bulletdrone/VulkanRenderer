#include "Precomp.h"
#include "Transform.h"

namespace Engine
{
	void Transform::RotateAxis(const glm::vec3& a_Axis, const float a_Angle, const Space a_Space)
	{
		glm::quat t_Quat(a_Angle, a_Axis.x, a_Axis.y, a_Axis.z);
		SetRotation(t_Quat, a_Space);
	}

	void Transform::SetPosition(const glm::vec3& a_Pos, const Space a_Space)
	{
		if (a_Space == Space::WORLD)
		{
			m_LocalPosition = a_Pos;
			if (m_Parent == nullptr) m_WorldPosition = m_LocalPosition;
			else { m_WorldPosition = (m_Parent->GetWorldRotation() * m_LocalPosition) + m_Parent->GetWorldPosition(); }
		}
		else
		{
			m_WorldPosition = a_Pos;
			if (m_Parent == nullptr) m_LocalPosition = a_Pos;
			else { m_LocalPosition = -m_Parent->GetWorldPosition(); }
		}
		m_DirtyMatrix = true;
	}

	void Transform::SetRotation(const glm::quat& a_Quat, const Space a_Space)
	{
		if (a_Space == Space::WORLD)
		{
			m_LocalRotation = a_Quat;
			if (m_Parent == nullptr) m_WorldRotation = a_Quat;
			else { m_WorldRotation = m_LocalRotation * m_Parent->GetWorldRotation(); }
		}
		else
		{
			m_WorldRotation = a_Quat;
			if (m_Parent == nullptr) m_LocalRotation = a_Quat;
			else { m_LocalRotation = glm::inverse(m_Parent->GetWorldRotation()) * a_Quat; }
		}
		m_DirtyMatrix = true;
	}

	void Transform::SetScale(const glm::vec3& a_Scale, const Space a_Space)
	{
		if (a_Space == Space::WORLD)
		{
			m_LocalScale = a_Scale;
			if (m_Parent == nullptr) m_WorldRotation = a_Scale;
			else { m_WorldScale = m_Parent->GetWorldScale() * m_LocalScale; }
		}
		else
		{
			m_WorldScale = a_Scale;
			if (m_Parent == nullptr) m_LocalScale = a_Scale;
			else { m_LocalScale = m_Parent->GetWorldScale() / a_Scale; }
		}
		m_DirtyMatrix = true;
	}
	const glm::mat4& Transform::GetModelMatrix()
	{
		if (!m_DirtyMatrix)
			return m_ModelMatrix;

		m_DirtyMatrix = false;
		return CreateModelMatrix();
	}

	const glm::mat4& Transform::CreateModelMatrix()
	{
		//--SETUP TRANSFORM MATRICES--
		glm::mat4 t_TranslationMatrix = glm::identity<glm::mat4>();
		glm::mat4 t_RotationMatrix = glm::identity<glm::mat4>();
		glm::mat4 t_ScaleMatrix = glm::identity<glm::mat4>();

		//Position
		t_TranslationMatrix = glm::translate(t_TranslationMatrix, m_WorldPosition);

		//Rotation is special because it needs to be send to the shader.
		t_RotationMatrix = glm::mat4_cast(m_WorldRotation);

		//Scale
		t_ScaleMatrix = glm::scale(t_ScaleMatrix, m_WorldScale);

		m_ModelMatrix = t_TranslationMatrix * t_RotationMatrix * t_ScaleMatrix;

		return m_ModelMatrix;
	}
}