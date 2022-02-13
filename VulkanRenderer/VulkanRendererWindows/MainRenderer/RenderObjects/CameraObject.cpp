#include "CameraObject.h"

CameraObject::CameraObject(Transform* a_Transform, const uint32_t a_CameraID, float a_Fov, float a_AspectRatio, float a_NearField, float a_FarField)
	:	p_Transform(a_Transform), m_CameraID(a_CameraID)
{
	m_Fov = a_Fov;
	m_AspectRatio = a_AspectRatio;
	m_NearField = a_NearField;
	m_FarField = a_FarField;

	m_ViewProjection.Projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearField, m_FarField);

	//Because of vulkan's cord system difference with OpenGL.
	m_ViewProjection.Projection[1][1] *= -1;
}

CameraObject::~CameraObject()
{}

void CameraObject::Update()
{
	

	// Direction math
	m_Direction.x = cos(glm::radians(p_Transform->Rotation.y + 90)) * cos(glm::radians(p_Transform->Rotation.x));
	m_Direction.y = sin(glm::radians(p_Transform->Rotation.x));
	m_Direction.z = sin(glm::radians(p_Transform->Rotation.y + 90)) * cos(glm::radians(p_Transform->Rotation.x));
	m_Direction = glm::normalize(m_Direction);

	//glm::vec3 lookAtTarget = Position() + m_Direction;
	glm::vec3 lookAtTarget = glm::vec3(0, 0, 0);

	// Calculate view
	//m_ViewProjection.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_ViewProjection.View = glm::lookAt(p_Transform->Position, lookAtTarget, m_ScreenOrientation);
}

void CameraObject::SetProjection(float a_Fov, float a_AspectRatio, float a_NearField, float a_FarField)
{
	m_Fov = a_Fov;
	m_AspectRatio = a_AspectRatio;
	m_NearField = a_NearField;
	m_FarField = a_FarField;

	m_ViewProjection.Projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearField, m_FarField);

	//Because of vulkan's cord system difference with OpenGL.
	m_ViewProjection.Projection[1][1] *= -1;
}
