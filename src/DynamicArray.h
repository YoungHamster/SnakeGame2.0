#pragma once

class DynamicArray
{
private:
	size_t sizeOfOneElement;
	size_t sizeOfArray;
	void* array;
public:
	DynamicArray(size_t sizeOfOneElement);
	~DynamicArray();
	void PushBack(void *element);
	void Resize(size_t newSize);
	void* operator[](int i);
};