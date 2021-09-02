#include "CameraController.h"

CameraController::CameraController(Renderer* a_Renderer)
	: p_Renderer(a_Renderer)
{
	if (CameraAvailable())
	{
		AddCamera(glm::vec3(2, 2, 2), glm::vec3(0), p_Renderer->GetAspectRatio(), 0.1f, 10.0f);
		SwitchCamera(0);

		AddCamera(glm::vec3(1, 1, 1), glm::vec3(0), p_Renderer->GetAspectRatio(), 0.1f, 10.0f);
	}
}

CameraController::~CameraController()
{}

void CameraController::AddCamera(glm::vec3 a_Position, glm::vec3 a_Rotationfloat, float a_AspectRatio, float a_NearField, float a_FarField)
{
	Transform* cameraTrans = new Transform(a_Position, a_Rotationfloat, glm::vec3(1));
	CameraObject* camera = new CameraObject(0, cameraTrans, m_Cameras.size(), m_StandardFov, a_AspectRatio, a_NearField, a_FarField);

	m_Cameras.push_back(camera);
}

bool CameraController::SwitchCamera(uint32_t a_CameraID)
{
	if (a_CameraID == m_ActiveCamera || a_CameraID > m_Cameras.size())
		return false;

	m_ActiveCamera = a_CameraID;

	p_Renderer->ReplaceActiveCamera(GetActiveCamera());
	return true;
}

uint32_t CameraController::SwitchNextCamera()
{
	m_ActiveCamera++;

	if (m_ActiveCamera >= m_Cameras.size())
		m_ActiveCamera = 0;

	p_Renderer->ReplaceActiveCamera(GetActiveCamera());

	return m_ActiveCamera;
}
