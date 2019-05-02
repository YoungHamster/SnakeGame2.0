#pragma once
#include <mutex>

template<typename T>
class AsyncArray
{
private:
	T* array;
	std::mutex lock;
public:
	AsyncArray(size_t arraySize);
	~AsyncArray();
	void Write(size_t i, T value);
	T Read(size_t i);
};
