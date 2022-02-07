#include "Precomp.h"
#include "MeshResource.h"

//#define TINYOBJLOADER_IMPLEMENTATION
#pragma warning (push, 0)
#define TINYOBJLOADER_IMPLEMENTATION
#include <ObjLoader/tiny_obj_loader.h>
#include <glm/glm.hpp>
#pragma warning (pop)
#include <unordered_map>

#include "ResourceAllocator.h"
#include "Renderer.h"

namespace Engine
{
	namespace Resource
	{
		MeshResource::MeshResource(const uint64_t a_HashID, ResourceAllocator& a_ResourceAllocator)
			: Resource(a_HashID, a_ResourceAllocator)
		{}

        MeshResource::~MeshResource()
        {
            Logger::Assert(!IsInitialized(), "MeshResource is still initialized.");
        }

		bool MeshResource::IsInitialized()
		{
            //WIP
            return false;
		}

		bool MeshResource::Load(const char* a_FilePath)
		{
            Resource::Load(a_FilePath);

            tinyobj::attrib_t t_Attrib;
            std::vector<tinyobj::shape_t> t_Shapes;
            std::vector<tinyobj::material_t> t_Materials;
            std::string t_Warning;
            std::string t_Error;

            // Read .obj file
            bool t_Success = tinyobj::LoadObj(&t_Attrib, &t_Shapes, &t_Materials, &t_Warning, &t_Error, a_FilePath, "", true);

            // Check for errors and warnings
            if (!t_Warning.empty())
            {
                printf(t_Warning.c_str());
            }
            if (!t_Error.empty())
            {
                Logger::Assert(false, (t_Error.c_str()));
            }
            if (!t_Success)
            {
                printf("Failed to load/parse .obj");
                return false;
            }

            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            std::unordered_map<Vertex, uint32_t> t_UniqueVertices;

            for (const auto& l_Shape : t_Shapes)
            {
                for (const auto& l_Index : l_Shape.mesh.indices)
                {
                    Vertex vertex{};

                    vertex.position = glm::vec3(
                        static_cast<float>(t_Attrib.vertices[3 * l_Index.vertex_index + 0]),
                        static_cast<float>(t_Attrib.vertices[3 * l_Index.vertex_index + 1]),
                        static_cast<float>(t_Attrib.vertices[3 * l_Index.vertex_index + 2]));

                    //vertex.normal = glm::vec3(
                    //    static_cast<float>(t_Attrib.normals[3 * l_Index.normal_index + 0]),
                    //    static_cast<float>(t_Attrib.normals[3 * l_Index.normal_index + 1]),
                    //    static_cast<float>(t_Attrib.normals[3 * l_Index.normal_index + 2]));

                    vertex.texCoord = glm::vec2(
                        static_cast<float>(t_Attrib.texcoords[2 * l_Index.texcoord_index + 0]),
                        1.0f - static_cast<float>(t_Attrib.texcoords[2 * l_Index.texcoord_index + 1]));

                    vertex.color = { 1.0f, 1.0f, 1.0f };


                    if (t_UniqueVertices.count(vertex) == 0) {
                        t_UniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                        vertices.push_back(vertex);
                    }

                    indices.push_back(t_UniqueVertices[vertex]);
                }
            }

            meshData.vertices = new BufferData<Vertex>(vertices);
            meshData.indices = new BufferData<uint32_t>(indices);

            //setup in renderer.
            r_ResourceAllocator.p_Renderer->SetupMesh(&meshData);

            return true;
		}

		bool MeshResource::Unload()
		{
            //WIP.
			return false;
		}
	}
}