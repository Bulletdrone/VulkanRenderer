#pragma once
#include <stdint.h>
#include <unordered_map>

struct Model;
class Material;

class Renderer;

namespace Engine
{
	typedef uint64_t hashindex;

	namespace Resource 
	{ 
		class Resource;

		enum class ResourceType
		{
			Texture,
			Mesh,
			Shader
		};
	}

	class ResourceAllocator
	{
	public: 
		static ResourceAllocator& GetInstance()
		{
			static ResourceAllocator instance;

			return instance;
		}

		//Safer for Singleton.
		ResourceAllocator(ResourceAllocator const&) = delete;
		void operator=(ResourceAllocator const&) = delete;

	public:
		ResourceAllocator();
		~ResourceAllocator();

		template <typename T>
		T& GetResource(hashindex a_ID);
		template <typename T>
		T& GetResource(const char* a_FilePath, Resource::ResourceType a_Type);

		hashindex LoadResource(const char* a_FilePath, Resource::ResourceType a_Type);

		void UnloadResource(const char* a_FilePath);
		void UnloadResource(hashindex a_ID);

		void SetRenderer(Renderer* a_Renderer) { p_Renderer = a_Renderer; }

	private:
		hashindex GetHashFromPath(const char* a_FilePath);

	private:
		std::unordered_map<hashindex, Resource::Resource*> m_Resources;

	public:
		Renderer* p_Renderer = nullptr;
	};

	template<typename T>
	inline T& ResourceAllocator::GetResource(hashindex a_ID)
	{
		Resource::Resource& t_Resource = m_Resources.at(a_ID);

		return *dynamic_cast<T*>(&t_Resource);
	}

	template<typename T>
	inline T& ResourceAllocator::GetResource(const char* a_FilePath, Resource::ResourceType a_Type)
	{
		hashindex t_HashIndex = GetHashFromPath(a_FilePath);

		auto t_Iterator = m_Resources.find(t_HashIndex);

		//If you cannot find it create and return the resource.
		if (t_Iterator != m_Resources.end())
		{
			return *dynamic_cast<T*>(m_Resources.at(t_HashIndex));
		}

		return *dynamic_cast<T*>(m_Resources.at(LoadResource(a_FilePath, a_Type)));
	}
};