#include "Precomp.h"
#include "TextureResource.h"

#include <stb/stb_image.h>

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
			if (m_PixelData != nullptr)
			{
				return true;
			}

			return false;
		}

		bool TextureResource::Load(const char* a_FilePath)
		{
			int t_Width, t_Height, t_Channels;
			m_PixelData = stbi_load(a_FilePath, &t_Width, &t_Height, &t_Channels, STBI_rgb_alpha);
			
			//If the loading fails.
			if (stbi_failure_reason() != NULL)
			{
				m_PixelData = nullptr;
				printf("Image loading Failed.");
				printf(stbi_failure_reason());
				return false;
			}
			
			//Convert to correct uin32_t values.
			m_TexWidth = static_cast<uint32_t>(t_Width);
			m_TexHeight = static_cast<uint32_t>(t_Height);
			m_TexChannels = static_cast<uint32_t>(t_Channels);

			constexpr uint32_t RGBASize = 4;
			m_ImageSize = static_cast<uint64_t>(m_TexWidth * m_TexHeight * RGBASize);

			return true;
		}

		bool TextureResource::Unload()
		{
			stbi_image_free(m_PixelData);
			m_PixelData = nullptr;

			printf("Texture might not been removed in Renderer.");

			return false;
		}

	}
}