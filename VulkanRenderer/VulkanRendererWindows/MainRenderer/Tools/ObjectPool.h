#pragma once
#include <stdint.h>

template <typename T>
class ObjectPool
{
public:
	ObjectPool();
	~ObjectPool();

	/*  Get a filled object on a array position.
	@param a_Pos, The position in the array.
	@return the object on the Position. */
	T& Get(uint32_t a_Pos);
	/*	Get an object that needs to be filled in. Call SetFilled with the r_Pos after you filled it in.
	@param r_IsFull, this value will be overwriten to see if the object was filled or not.
	@param a_Pos, The position in the array.
	@return the object regardless if it's filled. */
	T& GetFilledObject(bool& r_IsFull, uint32_t a_Pos);
	/*	Get a reference to a filled object. Returns false if the object is empty.
		@param r_Pos, this value will be overwriten with the object array position.
		@return An empty Object reference that needs to be filled in. */
	T& GetEmptyObject(uint32_t& r_Pos);

	const uint32_t Size() const { return size; }

private:
	//Increase the size of arr and isEmpty by the uint32_t addition.
	void AddExtraElements(uint32_t& r_Pos);

	//The Object Pool.
	T* arr;
	bool* isEmpty;

	uint32_t size = 8;
};

template <typename T>
inline ObjectPool<T>::ObjectPool()
{
	arr = new T[size];
	isEmpty = new bool[size];
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
inline T& ObjectPool<T>::GetFilledObject(bool& r_IsFull, uint32_t a_Pos)
{
	r_IsFull = !isEmpty[a_Pos];

	return arr[a_Pos];
}

template <typename T>
inline T& ObjectPool<T>::GetEmptyObject(uint32_t& r_Position)
{
	for (uint32_t i = 0; i < size; i++)
	{
		if (isEmpty[i])
		{
			isEmpty[i] = false;
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
	uint32_t newSize = size * 2;

	T* newArr = new T[newSize];
	bool* newIsEmpty = new bool[newSize] {false};

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
