#include "AsyncArray.h"

#include <Windows.h>

template<typename T>
AsyncArray<T>::AsyncArray(size_t arraySize)
{
	array = new T[arraySize];
}

template<typename T>
AsyncArray<T>::~AsyncArray()
{
	delete[] array;
}

template<typename T>
void AsyncArray<T>::Write(size_t i, T value)
{
	lock.lock();
	array[i] = value;
	lock.unlock();
}

template<typename T>
T AsyncArray<T>::Read(size_t i)
{
	lock.lock();
	return array[i];
	lock.unlock();
}