#pragma once
#include <vector>

#pragma warning (push, 0)
#include <Vulkan/vulkan.h>
#include <ObjLoader/tiny_obj_loader.h>
#pragma warning (pop)
#include <RenderObjects/Components/MeshData.h>

class ResourceLoader
{
public:
	static MeshData* LoadModel(const char* a_Filename);

	static std::vector<char> ReadFile(const std::string& filename);
};