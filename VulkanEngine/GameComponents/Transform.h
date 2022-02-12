#pragma once
#pragma warning (push, 0)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#pragma warning (pop)

//Credit to Honeycomb engine for some ideas on how to deal with parent transform.
//https://github.com/antverdovsky/Honeycomb-Game-Engine/blob/master/Honeycomb%20GE/src/component/physics/Transform.cpp

namespace Engine
{
	enum class Space
	{
		LOCAL,
		WORLD
	};

	class Transform
	{
	public:
		Transform() {};
		//Transform(glm::vec3 a_Pos = glm::vec3(0), glm::vec3 a_Rot = glm::vec3(0), glm::vec3 a_Scale = glm::vec3(1));
		~Transform() {};

		void SetParent(Transform* m_Transform);

		void RotateAxis(const glm::vec3& a_Axis, const float a_Angle, const Space a_Space);

		void SetPosition(const glm::vec3& a_Pos, const Space a_Space);
		void SetRotation(const glm::quat& a_Quat, const Space a_Space);
		void SetScale(const glm::vec3& a_Scale, const Space a_Space);

		const glm::vec3 GetWorldPosition() const { return m_WorldPosition; }
		const glm::quat GetWorldRotation() const { return m_WorldRotation; }
		const glm::vec3 GetWorldScale() const { return m_WorldScale; }

		const glm::mat4& GetModelMatrix();

	private:
		const glm::mat4& CreateModelMatrix();

		glm::vec3 m_LocalPosition = glm::vec3(0);
		glm::quat m_LocalRotation = glm::vec3(0);
		glm::vec3 m_LocalScale = glm::vec3(1);

		glm::vec3 m_WorldPosition = glm::vec3(0);
		glm::quat m_WorldRotation = glm::vec3(0);
		glm::vec3 m_WorldScale = glm::vec3(1);

		glm::mat4 m_ModelMatrix = glm::identity<glm::mat4>();
		bool m_DirtyMatrix = false;

		Transform* m_Parent = nullptr;
	};
}