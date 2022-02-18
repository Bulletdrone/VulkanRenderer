#include "pch.h"
#include "LL_HashMap.h"

Hash Hash::MakeHash(uint64_t a_Value, uint64_t a_MaxSize)
{
	return Hash((a_Value * 8 ^ 8) % a_MaxSize);
}

LL_HashMap::LL_HashMap()
{}

LL_HashMap::~LL_HashMap()
{
	for (auto& t_Element : m_Map)
	{
		delete t_Element;
	}
}

void LL_HashMap::Insert(hashindex a_Key, Engine::Resource::Resource* a_Res)
{
	Hash t_Hash = Hash::MakeHash(a_Key, standardSize);

	LL_HashEntry* entry = new LL_HashEntry;
	entry->key = a_Key;
	entry->value = a_Res;

	entry->next_Entry = m_Map[t_Hash];
	m_Map[t_Hash.hash] = entry;
}

Engine::Resource::Resource* LL_HashMap::Find(hashindex a_Key)
{
	Hash t_Hash = Hash::MakeHash(a_Key, standardSize);

	LL_HashEntry* entry = m_Map[t_Hash];
	while (entry)
	{
		if (match(entry, a_Key))
		{
			return entry->value;
		}
		entry = entry->next_Entry;
	}
	return nullptr;
}

bool LL_HashMap::match(LL_HashEntry* a_Entry, hashindex a_Key)
{
	if (a_Entry->key == a_Key)
	{
		return true;
	}
	return false;
}
