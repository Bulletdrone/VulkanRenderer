#pragma once
#include "Resource.h"

namespace Engine
{
	namespace Resource
	{
		class TextureResource : public Resource
		{
		public:
			TextureResource(const uint64_t a_HashID);
			~TextureResource();

			virtual bool IsInitialized();

			virtual bool Load(const char* a_FilePath);
			virtual bool Unload();
	

		private:
			unsigned char* m_PixelData = nullptr;

			//In Bytes
			uint64_t m_ImageSize = 0;

			//Metrics
			uint32_t m_TexWidth = 0;
			uint32_t m_TexHeight = 0;
			uint32_t m_TexChannels = 0;
		};
	}
}
