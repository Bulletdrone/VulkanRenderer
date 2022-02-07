#pragma once
#include "Resource.h"
#include "Structs/Texture.h"

class Renderer;

namespace Engine
{
	namespace Resource
	{
		class TextureResource : public Resource
		{
		public:
			TextureResource(const uint64_t a_HashID, ResourceAllocator& a_ResourceAllocator);
			~TextureResource();

			virtual bool IsInitialized();

			bool Load(const char* a_FilePath);
			virtual bool Unload();

			Texture texture;
		};
	}
}