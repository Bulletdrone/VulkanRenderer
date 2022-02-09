#include "BaseRenderObject.h"

BaseRenderObject::BaseRenderObject(const uint32_t a_ID, Transform* a_Transform, RenderHandle a_MatHandle)
	:	SceneObject(a_ID, a_Transform), m_MatHandle(a_MatHandle)
{
	UpdateModelMatrix();
}

BaseRenderObject::~BaseRenderObject()
{}

glm::mat4& BaseRenderObject::UpdateModelMatrix()
{
	return m_Model = CreateTranslationMatrix() * CreateRotationMatrix() * CreateScaleMatrix();
}

glm::mat4 BaseRenderObject::CreateTranslationMatrix()
{
	glm::mat4 t_TransMat = glm::mat4(1.0f);

	// this won't work (Brian's comment on this, it should work though.)
	t_TransMat[3][0] = Position().x;
	t_TransMat[3][1] = Position().y;
	t_TransMat[3][2] = Position().z;

	return t_TransMat;
}

glm::mat4 BaseRenderObject::CreateRotationMatrix()
{
		// set rotationx
	glm::mat4 t_RotationMatrixX = glm::mat4(1.0f);
	t_RotationMatrixX[1][1] = cosf(Rotation().x);
	t_RotationMatrixX[2][1] = -sinf(Rotation().x);
	t_RotationMatrixX[1][2] = sinf(Rotation().x);
	t_RotationMatrixX[2][2] = cosf(Rotation().x);
	//set rotationy
	glm::mat4 t_RotationMatrixY = glm::mat4(1.0f);
	t_RotationMatrixY[0][0] = cosf(Rotation().y);
	t_RotationMatrixY[2][0] = sinf(Rotation().y);
	t_RotationMatrixY[0][2] = -sinf(Rotation().y);
	t_RotationMatrixY[2][2] = cosf(Rotation().y);
	//set rotationz
	glm::mat4 t_RotationMatrixZ = glm::mat4(1.0f);
	t_RotationMatrixY[0][0] = cosf(Rotation().z);
	t_RotationMatrixY[1][0] = -sinf(Rotation().z);
	t_RotationMatrixY[0][1] = sinf(Rotation().z);
	t_RotationMatrixY[1][1] = cosf(Rotation().z);

	// now the combined rotation	
	return t_RotationMatrixX * t_RotationMatrixY * t_RotationMatrixZ;
}

glm::mat4 BaseRenderObject::CreateScaleMatrix()
{
	return glm::scale(glm::mat4(1), Scale());  // nice easy function;
}