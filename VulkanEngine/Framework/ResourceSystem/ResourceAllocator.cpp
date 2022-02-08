#include "Precomp.h"
#include "ResourceAllocator.h"

//ResourceType includes
#include "TextureResource.h"
#include "MeshResource.h"
#include "ShaderResource.h"

#include "Renderer.h"

namespace Engine
{
	ResourceAllocator::ResourceAllocator()
	{
	}

	ResourceAllocator::~ResourceAllocator()
	{
		for (auto& pair : m_Resources)
		{
			Logger::Assert(pair.second->Unload(), "Failed to Unload at Destructor of ResourceAllocator");
			delete pair.second;
		}
	}

	hashindex ResourceAllocator::LoadResource(const char* a_FilePath, Resource::ResourceType a_Type)
	{
		hashindex t_Hash = GetHashFromPath(a_FilePath);

		Resource::Resource* resource = nullptr;

		switch (a_Type)
		{
		case Resource::ResourceType::Texture:
			resource = new Resource::TextureResource(t_Hash, *this);
			break;
		case Resource::ResourceType::Mesh:
			resource = new Resource::MeshResource(t_Hash, *this);
			break;
		case Resource::ResourceType::Shader:
			resource = new Resource::ShaderResource(t_Hash, *this);
			break;
		default:
			Logger::Assert(false, "ResourceType is wrong to load in resource, doesn't exist or wrong casting.");
			break;
		}

		Logger::Assert(resource->Load(a_FilePath), "Failed to load resource");

		m_Resources.emplace(std::make_pair(t_Hash, resource));

		return t_Hash;
	}

	void ResourceAllocator::UnloadResource(const char* a_FilePath)
	{
		Logger::Assert(m_Resources.at(GetHashFromPath(a_FilePath))->Unload(), "Failed to unload Resource");
	}

	void ResourceAllocator::UnloadResource(hashindex a_ID)
	{
		Logger::Assert(m_Resources.at(a_ID)->Unload(), "Failed to unload Resource");
	}

	hashindex ResourceAllocator::GetHashFromPath(const char* a_FilePath)
	{
		uint64_t t_Hash = 0;

		while (*a_FilePath != '\0')
		{
			t_Hash += static_cast<uint64_t>(*a_FilePath++);
		}
		return t_Hash;
	}
}