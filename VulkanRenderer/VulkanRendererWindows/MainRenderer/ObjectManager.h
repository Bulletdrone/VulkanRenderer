#pragma once

#include "Renderer.h"
#include "RenderObjects/RenderFactories/RenderFactory.h"

class ObjectManager
{
public:
	ObjectManager(Renderer* a_Renderer);
	~ObjectManager();

	void UpdateObjects(float a_Dt);
	void CreateShape(ShapeType a_ShapeType, Transform* a_Transform);

	//Increases the m_CurrentRenderID by 1 and returns it.
	size_t GetNextRenderID() { return m_CurrentRenderID++; }

private:

	size_t m_CurrentRenderID = 0;

	std::vector<BaseRenderObject*> m_RenderObjects;

	Renderer* p_Renderer;
	RenderFactory* m_RenderFactory;


};

