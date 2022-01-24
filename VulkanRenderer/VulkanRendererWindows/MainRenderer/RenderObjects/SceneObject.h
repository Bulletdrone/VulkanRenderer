#pragma once
#include "Components/Transform.h"

class SceneObject
{
public:
	SceneObject(const uint32_t a_ID, Transform* a_Transform);
	virtual ~SceneObject();

	virtual void Update() = 0;

	//Get the Object's ID. Usually assosiated with a list or vector storage position.
	const uint32_t GetID() { return m_ID; }

	//Transform Getters
	Transform* GetTransform() const { return m_Transform; }

	glm::vec3& Position() { return m_Transform->Position; }
	glm::vec3& Rotation() { return m_Transform->Rotation; }
	glm::vec3& Scale() { return m_Transform->Scale; }

protected:
	const uint32_t m_ID;

	Transform* m_Transform = nullptr;
};

