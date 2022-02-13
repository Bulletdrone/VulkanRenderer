#pragma once
#include "Components/Transform.h"

struct ViewProjection
{
	alignas(16) glm::mat4 View;
	alignas(16) glm::mat4 Projection;
};

class CameraObject
{
public:
	CameraObject(Transform* a_Transform, const uint32_t a_CameraID, float a_Fov, float a_AspectRatio, float a_NearField, float a_FarField);
	virtual ~CameraObject();

	virtual void Update();

	void SetProjection(float a_Fov, float a_AspectRatio, float a_NearField, float a_FarField);

	//Getters
	const uint32_t GetCameraID() { return m_CameraID; };
	const ViewProjection& GetViewProjection() { return m_ViewProjection; }

protected:
	const uint32_t m_CameraID;

	ViewProjection m_ViewProjection;

	glm::vec3 m_Direction = glm::vec3(0);
	glm::vec3 m_ScreenOrientation = glm::vec3(0, 0, 1);

	float m_Fov;
	float m_AspectRatio;
	float m_NearField;
	float m_FarField;

	Transform* p_Transform = nullptr;
};

