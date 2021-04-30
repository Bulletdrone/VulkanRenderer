#include "ObjectManager.h"

ObjectManager::ObjectManager(Renderer* a_Renderer)
	:	p_Renderer(a_Renderer)
{}

ObjectManager::~ObjectManager()
{
}

void ObjectManager::UpdateObjects(float a_Dt)
{
	uint32_t t_ImageIndex;
	p_Renderer->DrawFrame(t_ImageIndex);
}
