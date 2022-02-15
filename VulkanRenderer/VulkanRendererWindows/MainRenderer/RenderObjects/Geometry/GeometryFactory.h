#pragma once
#include <vector>
#include <Tools/RenderHandle.h>

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
	MeshHandle GetShape(GeometryType a_Type);
	MaterialHandle GetDebugMaterial() { return m_DebugMaterialHandle; }

private:
	MeshHandle m_ShapeHandles[static_cast<size_t>(GeometryType::Count)];
	MaterialHandle m_DebugMaterialHandle = RENDER_NULL_HANDLE;

	const unsigned char* LoadShader(const char* a_FilePath, size_t& a_ShaderSize);
};