#pragma once

#include "Renderer.h"

class BaseRenderObject;

class ObjectManager
{
public:
	ObjectManager(Renderer* a_Renderer);

	void UpdateObjects(float a_Dt);

public:
	Renderer* p_Renderer;
};