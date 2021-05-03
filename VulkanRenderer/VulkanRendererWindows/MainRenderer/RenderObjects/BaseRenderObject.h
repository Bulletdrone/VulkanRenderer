#pragma once
#include <MeshData.h>
#include "Components/Transform.h"

class BaseRenderObject
{
public:
	BaseRenderObject(const size_t a_ID, Transform* a_Transform);
	~BaseRenderObject();

	virtual void Update() = 0;
	virtual void Draw() = 0;

	//ModelMatrix functions.
	virtual glm::mat4& UpdateModelMatrix();

	glm::mat4 CreateTranslationMatrix();
	glm::mat4 CreateRotationMatrix();
	glm::mat4 CreateScaleMatrix();

	//Getters
	MeshData* GetMeshData() const { return m_MeshData; }
	Transform* GetTransform() const { return m_Transform; }

protected:
	const size_t m_ID;

	//Matrices.
	glm::mat4 m_Model;

	//Position, Rotation and Scaledata.
	Transform* m_Transform = nullptr;

	//Vertex Data with it's buffers.
	MeshData* m_MeshData = nullptr;
};