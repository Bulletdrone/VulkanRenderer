#pragma once
#include <vector>

#include <Vulkan/vulkan.hpp>

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
	void Cleanup(VkDevice& a_VkDevice);

	MeshData* GetShape(GeometryType a_Type);

private:
	std::vector<MeshData*> m_ShapeData;
};