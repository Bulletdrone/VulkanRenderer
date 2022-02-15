#pragma once
#include "Resource.h"
#include "Tools/RenderHandle.h"

namespace Engine
{
	namespace Resource
	{

		class ShaderResource : public Resource
		{
		public:
			ShaderResource(const uint64_t a_HashID, ResourceAllocator& a_ResourceAllocator);
			~ShaderResource();

			virtual bool IsInitialized();

			bool Load(const char* a_FilePath);
			virtual bool Unload();

			ShaderHandle shaderHandle;
		};
	}
}