#pragma once
#include <fstream>
#include <vector>

class ResourceLoader
{
public:
	static std::vector<char> ReadFile(const std::string& filename);
};