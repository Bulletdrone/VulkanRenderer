#pragma once

#include "Renderer.h"

class ObjectManager
{
public:
	ObjectManager(Renderer* a_Renderer);
	~ObjectManager();

	void UpdateObjects(float a_Dt);

private:
	Renderer* p_Renderer;
};

