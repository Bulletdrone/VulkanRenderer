#define TINYOBJLOADER_IMPLEMENTATION
#include "ResourceLoader.h"

#include <fstream>
#include <iostream>

#include <unordered_map>

//MeshData* ResourceLoader::LoadModel(const char* a_Filename)
//{
//    tinyobj::attrib_t t_Attrib;
//    std::vector<tinyobj::shape_t> t_Shapes;
//    std::vector<tinyobj::material_t> t_Materials;
//    std::string t_Warning;
//    std::string t_Error;
//
//    // Read .obj file
//    bool t_Success = tinyobj::LoadObj(&t_Attrib, &t_Shapes, &t_Materials, &t_Warning, &t_Error, a_Filename, "", true);
//
//    // Check for errors and warnings
//    if (!t_Warning.empty())
//    {
//        std::cout << "Warning:" + t_Warning << std::endl;
//    }
//    if (!t_Error.empty())
//    {
//        std::cout << "Error:" + t_Error << std::endl;
//    }
//    if (!t_Success)
//    {
//        std::cout << "Failed to load/parse .obj" << std::endl;
//    }
//
//
//    std::vector<Vertex> vertices;
//    std::vector<uint32_t> indices;
//
//    std::unordered_map<Vertex, uint32_t> t_UniqueVertices;
//
//    for (const auto& l_Shape : t_Shapes)
//    {
//        for (const auto& l_Index : l_Shape.mesh.indices)
//        {
//            Vertex vertex;
//
//            vertex.position = glm::vec3(
//                static_cast<float>(t_Attrib.vertices[3 * l_Index.vertex_index + 0]),
//                static_cast<float>(t_Attrib.vertices[3 * l_Index.vertex_index + 1]),
//                static_cast<float>(t_Attrib.vertices[3 * l_Index.vertex_index + 2]));
//
//            //vertex.normal = glm::vec3(
//            //    static_cast<float>(t_Attrib.normals[3 * l_Index.normal_index + 0]),
//            //    static_cast<float>(t_Attrib.normals[3 * l_Index.normal_index + 1]),
//            //    static_cast<float>(t_Attrib.normals[3 * l_Index.normal_index + 2]));
//
//            vertex.texCoord = glm::vec2(
//                static_cast<float>(t_Attrib.texcoords[2 * l_Index.texcoord_index + 0]),
//                1.0f - static_cast<float>(t_Attrib.texcoords[2 * l_Index.texcoord_index + 1]));
//
//            vertex.color = { 1.0f, 1.0f, 1.0f };
//
//
//
//            if (t_UniqueVertices.count(vertex) == 0) {
//                t_UniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
//                vertices.push_back(vertex);
//            }
//
//            indices.push_back(t_UniqueVertices[vertex]);
//        }
//    }
//
//    MeshData* t_MeshData = new MeshData();
//
//    t_MeshData->vertices = new BufferData<Vertex>(vertices);
//    t_MeshData->indices = new BufferData<uint32_t>(indices);
//
//    return t_MeshData;
//}

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