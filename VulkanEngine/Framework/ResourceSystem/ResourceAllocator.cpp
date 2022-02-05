#include "Precomp.h"
#include "ResourceAllocator.h"

//ResourceType includes
#include "TextureResource.h"
#include "MeshResource.h"
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

	HashIndex ResourceAllocator::LoadResource(const char* a_FilePath, Resource::ResourceType a_Type)
	{
		auto t_Iterator = m_Resources.find(GetHashFromPath(a_FilePath));
		switch (a_Type)
		{
		case Resource::ResourceType::Texture:
			return CreateTexture(a_FilePath);
			break;
		case Resource::ResourceType::Mesh:
			return CreateModel(a_FilePath);
			break;
		default:
			Logger::Assert(false, "ResourceType is wrong to load in resource, doesn't exist or wrong casting.");
			break;
		}

		return 0;
	}

	void ResourceAllocator::UnloadResource(const char* a_FilePath)
	{
		Logger::Assert(m_Resources.at(GetHashFromPath(a_FilePath))->Unload(), "Failed to unload Resource");
	}

	void ResourceAllocator::UnloadResource(HashIndex a_ID)
	{
		Logger::Assert(m_Resources.at(a_ID)->Unload(), "Failed to unload Resource");
	}


	HashIndex ResourceAllocator::CreateTexture(const char* a_FilePath)
	{
		HashIndex t_Index = GetHashFromPath(a_FilePath);
		Resource::TextureResource* t_Texture = new Resource::TextureResource(t_Index);
		
		t_Texture->Load(a_FilePath);
		p_Renderer->SetupImage(t_Texture->texture);
		m_Resources.emplace(std::make_pair(t_Index, t_Texture));

		return t_Index;
	}

	HashIndex ResourceAllocator::CreateModel(const char* a_FilePath)
	{
		HashIndex t_Index = GetHashFromPath(a_FilePath);

		Resource::MeshResource* t_Mesh = new Resource::MeshResource(t_Index);

		t_Mesh->Load(a_FilePath);
		p_Renderer->SetupMesh(&t_Mesh->meshData);
		m_Resources.emplace(std::make_pair(t_Index, t_Mesh));

		return t_Index;
	}

	HashIndex ResourceAllocator::GetHashFromPath(const char* a_FilePath)
	{
		uint64_t t_Hash = 0;

		while (*a_FilePath != '\0')
		{
			t_Hash += static_cast<uint64_t>(*a_FilePath++);
		}
		return t_Hash;
	}
}