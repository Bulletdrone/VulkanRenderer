#include "BaseRenderObject.h"

BaseRenderObject::BaseRenderObject(const size_t a_ID, Transform* a_Transform)
	:	m_ID(a_ID)
{
	m_Transform = a_Transform;
	UpdateModelMatrix();
}

BaseRenderObject::~BaseRenderObject()
{
	delete m_Transform;
}

glm::mat4& BaseRenderObject::UpdateModelMatrix()
{
	return m_Model = CreateTranslationMatrix() * CreateRotationMatrix() * CreateScaleMatrix();
}

glm::mat4 BaseRenderObject::CreateTranslationMatrix()
{
	glm::mat4 t_TransMat = glm::mat4(1.0f);

	// this won't work (Brian's comment on this, it should work though.)
	t_TransMat[3][0] = m_Transform->Position.x;
	t_TransMat[3][1] = m_Transform->Position.y;
	t_TransMat[3][2] = m_Transform->Position.z;

	return t_TransMat;
}

glm::mat4 BaseRenderObject::CreateRotationMatrix()
{
		// set rotationx
	glm::mat4 t_RotationMatrixX = glm::mat4(1.0f);
	t_RotationMatrixX[1][1] = cosf(m_Transform->Rotation.x);
	t_RotationMatrixX[2][1] = -sinf(m_Transform->Rotation.x);
	t_RotationMatrixX[1][2] = sinf(m_Transform->Rotation.x);
	t_RotationMatrixX[2][2] = cosf(m_Transform->Rotation.x);
	//set rotationy
	glm::mat4 t_RotationMatrixY = glm::mat4(1.0f);
	t_RotationMatrixY[0][0] = cosf(m_Transform->Rotation.y);
	t_RotationMatrixY[2][0] = sinf(m_Transform->Rotation.y);
	t_RotationMatrixY[0][2] = -sinf(m_Transform->Rotation.y);
	t_RotationMatrixY[2][2] = cosf(m_Transform->Rotation.y);
	//set rotationz
	glm::mat4 t_RotationMatrixZ = glm::mat4(1.0f);
	t_RotationMatrixY[0][0] = cosf(m_Transform->Rotation.z);
	t_RotationMatrixY[1][0] = -sinf(m_Transform->Rotation.z);
	t_RotationMatrixY[0][1] = sinf(m_Transform->Rotation.z);
	t_RotationMatrixY[1][1] = cosf(m_Transform->Rotation.z);

	// now the combined rotation	
	return t_RotationMatrixX * t_RotationMatrixY * t_RotationMatrixZ;
}

glm::mat4 BaseRenderObject::CreateScaleMatrix()
{
	return glm::scale(glm::mat4(1), m_Transform->Scale);  // nice easy function;
}