#pragma once
#include "RenderObjects/CameraObject.h"
#include "Renderer.h"

#include <vector>

class CameraController
{
public:
	CameraController(Renderer* a_Renderer);
	~CameraController();

	void AddCamera(glm::vec3 a_Position, glm::vec3 a_Rotation, float a_AspectRatio, float a_NearField, float a_FarField);
	
	//returns false if no camera with that ID exists.
	//bool CopyCamera(uint32_t a_CameraID);

	//returns true if switch was possible.
	bool SwitchCamera(uint32_t a_CameraID);

	void UpdateActiveCamera() { GetActiveCamera()->Update(); }

	//Get the current active Camera.
	CameraObject* GetActiveCamera() { return m_Cameras[m_ActiveCamera]; }

	//Is there an camera active?
	bool CameraAvailable() { return m_Cameras.empty(); };


private:
	uint32_t m_ActiveCamera = MAXUINT32;
	std::vector<CameraObject*> m_Cameras;

	//can be changed during runtime.
	float m_StandardFov = 45.0f;

	Renderer* p_Renderer;
};
