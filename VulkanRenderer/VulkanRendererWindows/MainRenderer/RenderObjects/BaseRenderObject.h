#pragma once

#include "SceneObject.h"

#include "Components/RenderObjectData.h"
#include <Structs/PipelineData.h>

class BaseRenderObject : public SceneObject
{
public:
	BaseRenderObject(const uint32_t a_ID, Transform* a_Transform, uint32_t a_PipelineID);
	~BaseRenderObject();

	virtual void Update() = 0;
	virtual void Draw() = 0;

	//ModelMatrix functions.
	virtual glm::mat4& UpdateModelMatrix();

	glm::mat4 CreateTranslationMatrix();
	glm::mat4 CreateRotationMatrix();
	glm::mat4 CreateScaleMatrix();

	//Getters
	const glm::mat4& GetModelMatrix() const { return m_Model; }

	uint32_t GetPipeLineID() const { return m_PipeLineID; }
	MeshData* GetMeshData() const { return p_RenderObjectData->m_MeshData; }

	BufferData<Vertex>* GetVertexData() const { return p_RenderObjectData->m_MeshData->GetVertexData(); }
	BufferData<uint32_t>* GetIndexData() const { return p_RenderObjectData->m_MeshData->GetIndexData(); }

protected:
	//Matrices.
	glm::mat4 m_Model;

	//Vertex Data with it's buffers.
	RenderObjectData* p_RenderObjectData = nullptr;
	//PipelineData.
	uint32_t m_PipeLineID;
};