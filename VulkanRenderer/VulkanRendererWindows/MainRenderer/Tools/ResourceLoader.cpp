#include "ResourceLoader.h"

std::vector<char> ResourceLoader::ReadFile(const std::string& filename) {
    std::ifstream t_File(filename, std::ios::ate | std::ios::binary);
        
    if (!t_File.is_open()) 
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t t_FileSize = (size_t)t_File.tellg();
    std::vector<char> t_Buffer(t_FileSize);

    t_File.seekg(0);
    t_File.read(t_Buffer.data(), t_FileSize);

    t_File.close();

    return t_Buffer;
}