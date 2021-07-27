#pragma once
#include <stdint.h>

template <typename T>
class ObjectPool
{
public:
	/*  @param a_Size: the initial Size of the pool.
		@param a_Addition: the increase of the size when no empty objects exist. 0 becomes a 1. */
	ObjectPool(uint32_t a_Size, uint32_t a_Addition);
	~ObjectPool();

	/*  Get a filled object on a array position.
	@param a_Pos, The position in the array.
	@return the object on the Position. */
	T& Get(uint32_t a_Pos);
	/*	Get an object that needs to be filled in. Call SetFilled with the r_Pos after you filled it in.
	@param r_Object, this value will be overwriten with an filled object is it was filled, otherwise it won't change.
	@param a_Pos, The position in the array.
	@return If the object was filled. */
	bool GetFilledObject(T& r_Object, uint32_t a_Pos);
	/*	Get a reference to a filled object. Returns false if the object is empty.
		@param r_Pos, this value will be overwriten with the object array position.
		@return An empty Object reference that needs to be filled in. */
	T& GetEmptyObject(uint32_t& r_Pos);

	const uint32_t Size() { return size; }

private:
	//Increase the size of arr and isEmpty by the uint32_t addition.
	void AddExtraElements(uint32_t& r_Pos);

	//The Object Pool.
	T* arr;
	bool* isEmpty;

	uint32_t size;
	uint32_t addition;
};

template <typename T>
inline ObjectPool<T>::ObjectPool(uint32_t a_Size, uint32_t a_Addition)
{
	size = a_Size;
	if (a_Addition != 0)
		addition = a_Addition;
	else
		addition = 1;

	arr = new T[size];
	isEmpty = new bool[size] {true};
}

template <typename T>
inline ObjectPool<T>::~ObjectPool()
{
	delete[] arr;
	delete[] isEmpty;
}

template <typename T>
inline T& ObjectPool<T>::Get(uint32_t a_Pos)
{
	return arr[a_Pos];
}

template<typename T>
inline bool ObjectPool<T>::GetFilledObject(T& r_Object, uint32_t a_Pos)
{
	if (isEmpty(a_Pos))
		return false;

	r_Object = arr[a_Pos];
	return true;
}

template <typename T>
inline T& ObjectPool<T>::GetEmptyObject(uint32_t& r_Position)
{
	for (uint32_t i = 0; i < size; i++)
	{
		if (isEmpty)
		{
			isEmpty = false;
			r_Position = i;
			return arr[i];
		}
	}

	AddExtraElements(r_Position);
	return arr[r_Position];
}

template <typename T>
inline void ObjectPool<T>::AddExtraElements(uint32_t& r_Position)
{
	uint32_t newSize = size + addition;

	T* newArr = new T[newSize];
	bool newIsEmpty = new bool[newSize] {false};

	for (uint32_t i = 0; i < size; i++)
	{
		newArr[i] = arr[i];
		newIsEmpty[i] = isEmpty[i];
	}

	r_Position = size + 1;
	size = newSize;

	delete[] arr;
	delete[] isEmpty;

	arr = newArr;
}
