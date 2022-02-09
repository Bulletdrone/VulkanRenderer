#pragma once

#include "SceneObject.h"
#include "Tools/RenderHandle.h"

#include "Components/MeshData.h"
#include "Components/Material.h"
#include <Structs/PipelineData.h>

class BaseRenderObject : public SceneObject
{
public:
	BaseRenderObject(const uint32_t a_ID, Transform* a_Transform, RenderHandle a_MatHandle);
	virtual ~BaseRenderObject();

	virtual void Update() = 0;
	virtual void Draw() = 0;

	//ModelMatrix functions.
	virtual glm::mat4& UpdateModelMatrix();

	glm::mat4 CreateTranslationMatrix();
	glm::mat4 CreateRotationMatrix();
	glm::mat4 CreateScaleMatrix();

	//Getters
	const glm::mat4& GetModelMatrix() const { return m_Model; }

	const RenderHandle GetMaterialHandle() const { return m_MatHandle; }
	const RenderHandle GetMeshHandle() const { return m_MeshHandle; }

protected:
	//Matrices.
	glm::mat4 m_Model = glm::mat4(0);

	RenderHandle m_MeshHandle = 0;
	RenderHandle m_MatHandle = RENDER_NULL_HANDLE;
};