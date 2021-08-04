#pragma once
#include <vector>

#include <Vulkan/vulkan.h>
#include <ObjLoader/tiny_obj_loader.h>
#include <RenderObjects/Components/MeshData.h>

class ResourceLoader
{
public:
	static MeshData* LoadModel(const char* a_Filename);

	static std::vector<char> ReadFile(const std::string& filename);
};