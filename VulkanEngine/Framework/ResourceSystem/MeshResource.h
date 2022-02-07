#pragma once
#include "Resource.h"
#include "RenderObjects/Components/MeshData.h"

namespace Engine
{
	namespace Resource
	{
		class MeshResource : public Resource
		{
		public:
			MeshResource(const uint64_t a_HashID, ResourceAllocator& a_ResourceAllocator);
			~MeshResource();

			virtual bool IsInitialized();

			virtual bool Load(const char* a_FilePath);
			virtual bool Unload();

			MeshData meshData{};
		};
	}
}