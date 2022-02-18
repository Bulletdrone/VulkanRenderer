#pragma once
#include <cstdint>
#include <cassert>

struct Hash
{
	Hash() {};
	uint64_t hash = 0;

	operator const uint64_t() const { return hash; }
	void operator=(const uint64_t a_Rhs) { hash = a_Rhs;}

	//Create with uint64_t.
	template<typename T>
	static Hash MakeHash(T a_Value);

	template <uint64_t>
	static Hash MakeHash(uint64_t a_Value);

private:
	Hash(uint64_t a_Hash) : hash(a_Hash) {};
};

template<typename T>
inline Hash Hash::MakeHash(T a_Value)
{
	assert("unsupported hash type");
	return Hash();
}

template<>
inline Hash Hash::MakeHash(uint64_t a_Value)
{
	return Hash(a_Value * 8 ^ 8);
}