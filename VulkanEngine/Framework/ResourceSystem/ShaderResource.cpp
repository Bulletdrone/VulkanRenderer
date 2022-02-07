#include "Precomp.h"
#include "ShaderResource.h"

#include "ResourceAllocator.h"
#include "Renderer.h"

namespace Engine
{
	namespace Resource
	{
		ShaderResource::ShaderResource(const uint64_t a_HashID, ResourceAllocator& a_ResourceAllocator)
			: Resource(a_HashID, a_ResourceAllocator)
		{}

		ShaderResource::~ShaderResource()
		{
			Logger::Assert(!IsInitialized(), "ShaderResource is still initialized.");
		}

		bool ShaderResource::IsInitialized()
		{
			if (shader.shaderModule != VK_NULL_HANDLE)
			{
				return true;
			}
			return false;
		}

		bool ShaderResource::Load(const char* a_FilePath)
		{
			Resource::Load(a_FilePath);

			Buffer t_Buffer = ReadFile(a_FilePath);

			shader = r_ResourceAllocator.p_Renderer->CreateShader(t_Buffer.data, t_Buffer.size);

			return true;
		}

		bool ShaderResource::Unload()
		{
			return true;
		}
	}
}