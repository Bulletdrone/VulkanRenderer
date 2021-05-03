#pragma once
#include "RenderShape.h"

enum class ShapeType
{
	Triangle,
	Rectangle
};

class RenderFactory
{
public:
	static BaseRenderObject* CreateShape(const size_t a_RenderID, ShapeType a_ShapeType, Transform* a_Transform);
};

