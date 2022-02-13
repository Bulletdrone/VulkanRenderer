#include "ObjectManager.h"
#include "Tools/VulkanInitializers.h"

ObjectManager::ObjectManager(Renderer* a_Renderer)
	: p_Renderer(a_Renderer)
{}

void ObjectManager::UpdateObjects(float a_Dt)
{
	(void)a_Dt;
	uint32_t t_ImageIndex;

	p_Renderer->DrawFrame(t_ImageIndex);
}