#include "Precomp.h"
#include "TextureResource.h"

#pragma warning (push, 0)
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#pragma warning (pop)

#include "ResourceAllocator.h"
#include "Renderer.h"

namespace Engine
{
	namespace Resource
	{
		TextureResource::TextureResource(const uint64_t a_HashID, ResourceAllocator& a_ResourceAllocator)
			: Resource(a_HashID, a_ResourceAllocator)
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
			unsigned char* t_data = stbi_load(a_FilePath, &t_Width, &t_Height, &t_Channels, STBI_rgb_alpha);

			//If the loading fails.
			if (t_data == NULL)
			{
				t_data = nullptr;
				printf("Image loading Failed.");
				printf(stbi_failure_reason());
				return false;
			}
			
			//Convert to correct uin32_t values.
			texture.texWidth = static_cast<uint32_t>(t_Width);
			texture.texHeight = static_cast<uint32_t>(t_Height);
			texture.texChannels = static_cast<uint32_t>(t_Channels);


			//Setup in renderer.
			r_ResourceAllocator.p_Renderer->SetupImage(texture, t_data);

			//It's loaded in the GPU so remove the data.
			stbi_image_free(t_data);

			return true;
		}

		bool TextureResource::Unload()
		{
			printf("Texture might not been removed in Renderer.");

			return false;
		}
	}
}