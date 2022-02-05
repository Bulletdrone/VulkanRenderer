#include "Precomp.h"
#include "TextureResource.h"

#pragma warning (push, 0)
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#pragma warning (pop)

namespace Engine
{
	namespace Resource
	{
		TextureResource::TextureResource(const uint64_t a_HashID)
			: Resource(a_HashID)
		{}

		TextureResource::~TextureResource()
		{
			Logger::Assert(!IsInitialized(), "TextureResource is still initialized.");
		}

		bool TextureResource::IsInitialized()
		{
			if (texture.textureImage != VK_NULL_HANDLE)
			{
				return true;
			}

			return false;
		}

		bool TextureResource::Load(const char* a_FilePath)
		{
			int t_Width, t_Height, t_Channels;
			texture.pixelData = stbi_load(a_FilePath, &t_Width, &t_Height, &t_Channels, STBI_rgb_alpha);
			
			//If the loading fails.
			if (texture.pixelData == NULL)
			{
				texture.pixelData = nullptr;
				printf("Image loading Failed.");
				printf(stbi_failure_reason());
				return false;
			}
			
			//Convert to correct uin32_t values.
			texture.texWidth = static_cast<uint32_t>(t_Width);
			texture.texHeight = static_cast<uint32_t>(t_Height);
			texture.texChannels = static_cast<uint32_t>(t_Channels);

			return true;
		}

		bool TextureResource::Unload()
		{
			stbi_image_free(texture.pixelData);
			texture.pixelData = nullptr;

			printf("Texture might not been removed in Renderer.");

			return false;
		}
	}
}