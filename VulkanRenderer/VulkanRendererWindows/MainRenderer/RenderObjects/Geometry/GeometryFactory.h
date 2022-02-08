#pragma once
#include <vector>

#pragma warning (push, 0)
#include <Vulkan/vulkan.h>
#pragma warning (pop)

struct MeshData;
class Renderer;

enum class GeometryType
{
	Triangle = 0,
	Quad = 1,
	//How many shape types there are available, never use this on GetShape function of the GeometryFactory.
	Count = 2
};

//This class has basic geometry
class GeometryFactory
{
public:
	void Init(Renderer* a_Renderer);
	uint32_t GetShape(GeometryType a_Type);

private:
	uint32_t m_ShapeHandlers[static_cast<size_t>(GeometryType::Count)];

};