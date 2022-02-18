#pragma once
#include <cstdint>

constexpr uint32_t standardSize = 1024;

typedef uint64_t hashindex;
namespace Engine
{

	namespace Resource
	{
		class Resource;
	}
}

struct Hash
{
	uint64_t hash;

	operator const uint64_t() const { return hash; }

	//Create with uint64_t.
	static Hash MakeHash(uint64_t a_Value, uint64_t a_MaxSize);

private:
	Hash(uint64_t a_Hash) : hash(a_Hash) {};
};
	
struct LL_HashEntry
{
	LL_HashEntry* next_Entry = nullptr;
	hashindex key{};
	Engine::Resource::Resource* value = nullptr;
};


//template<typename Key, typename Value>
class LL_HashMap
{
public:
	LL_HashMap();
	~LL_HashMap();

	void Insert(hashindex a_Key, Engine::Resource::Resource* a_Res);
	Engine::Resource::Resource* Find(hashindex a_Key);

private:
	bool match(LL_HashEntry* a_Entry, hashindex a_Key);


	LL_HashEntry* m_Map[standardSize]{ nullptr };
};


