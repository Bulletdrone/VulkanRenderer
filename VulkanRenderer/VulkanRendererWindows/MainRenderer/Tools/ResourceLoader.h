#pragma once
#include <fstream>
#include <vector>

#include <Vulkan/vulkan.h>

class ResourceLoader
{
public:
	static std::vector<char> ReadFile(const std::string& filename);
};