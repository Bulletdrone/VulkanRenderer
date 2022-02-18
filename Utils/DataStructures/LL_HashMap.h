#pragma once
#include "hash.h"

constexpr uint32_t STANDARDHASHMAPSIZE = 1024;


template<typename Key, typename Value>
struct LL_HashEntry
{
	LL_HashEntry* next_Entry = nullptr;

	Key key{};
	Value value{};
};


template<typename Key, typename Value>
class LL_HashMap
{
public:
	LL_HashMap();
	~LL_HashMap();

	void Insert(Key a_Key, Value a_Res);
	Value Find(Key a_Key);

private:
	bool Match(LL_HashEntry<Key, Value>* a_Entry, Key a_Key);


	LL_HashEntry<Key, Value>* m_Map[STANDARDHASHMAPSIZE]{ nullptr };
};

template<typename Key, typename Value>
LL_HashMap<Key, Value>::LL_HashMap()
{}

template<typename Key, typename Value>
LL_HashMap<Key, Value>::~LL_HashMap()
{
	for (auto& t_Element : m_Map)
	{
		delete t_Element;
	}
}

template<typename Key, typename Value>
void LL_HashMap<Key, Value>::Insert(Key a_Key, Value a_Res)
{
	Hash t_Hash = Hash::MakeHash(a_Key);
	t_Hash.hash = t_Hash % STANDARDHASHMAPSIZE;

	LL_HashEntry<Key, Value>* entry = new LL_HashEntry<Key, Value>;
	entry->key = a_Key;
	entry->value = a_Res;

	entry->next_Entry = m_Map[t_Hash];
	m_Map[t_Hash.hash] = entry;
}

template<typename Key, typename Value>
Value LL_HashMap<Key, Value>::Find(Key a_Key)
{
	Hash t_Hash = Hash::MakeHash(a_Key);
	t_Hash = t_Hash % STANDARDHASHMAPSIZE;

	LL_HashEntry<Key, Value>* entry = m_Map[t_Hash];
	while (entry)
	{
		if (Match(entry, a_Key))
		{
			return entry->value;
		}
		entry = entry->next_Entry;
	}
	return nullptr;
}

template<typename Key, typename Value>
bool LL_HashMap<Key, Value>::Match(LL_HashEntry<Key, Value>* a_Entry, Key a_Key)
{
	if (a_Entry->key == a_Key)
	{
		return true;
	}
	return false;
}