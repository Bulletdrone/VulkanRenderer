#include "CameraObject.h"

CameraObject::CameraObject(const uint32_t a_ID, Transform* a_Transform, const uint32_t a_CameraID, float a_Fov, float a_AspectRatio, float a_NearField, float a_FarField)
	:	SceneObject(a_ID, a_Transform), m_CameraID(a_CameraID)
{
	m_Fov = a_Fov;
	m_AspectRatio = a_AspectRatio;
	m_NearField = a_NearField;
	m_FarField = a_FarField;

	m_ViewProjection.Projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearField, m_FarField);
	m_ScreenOrientation = glm::vec3(0, 1, 0);
}

CameraObject::~CameraObject()
{
}

void CameraObject::Update()
{
	// Direction math
	m_Direction.x = cos(glm::radians(Rotation().y + 90)) * cos(glm::radians(Rotation().x));
	m_Direction.y = sin(glm::radians(Rotation().x));
	m_Direction.z = sin(glm::radians(Rotation().y + 90)) * cos(glm::radians(Rotation().x));
	m_Direction = glm::normalize(m_Direction);

	glm::vec3 lookAtTarget = Position() + m_Direction;

	// Calculate view
	m_ViewProjection.View = glm::lookAt(Position(), lookAtTarget, m_ScreenOrientation);
}

void CameraObject::SetProjection(float a_Fov, float a_AspectRatio, float a_NearField, float a_FarField)
{
	m_Fov = a_Fov;
	m_AspectRatio = a_AspectRatio;
	m_NearField = a_NearField;
	m_FarField = a_FarField;

	m_ViewProjection.Projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearField, m_FarField);
}
