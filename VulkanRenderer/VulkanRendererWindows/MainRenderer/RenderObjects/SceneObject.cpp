#include "SceneObject.h"

SceneObject::SceneObject(const uint32_t a_ID, Transform* a_Transform)
	:	m_ID(a_ID)
{
	m_Transform = a_Transform;
}

SceneObject::~SceneObject()
{
	delete m_Transform;
}
