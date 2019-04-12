#pragma once

class DynamicArray
{
private:
	size_t sizeOfOneElement = 0;
	size_t meanNumberOfElements = 0;
	size_t realNumberOfElements = 0;
	void* array = nullptr;
public:
	DynamicArray(size_t sizeOfOneElement);
	DynamicArray(size_t sizeOfOneElement, size_t numberOfElements);
	~DynamicArray();
	void PushBack(void *const element);
	void PopBack();
	void Resize(size_t newNumberOfElements);
	void Delete(int where);
	void Delete(int firstElement, int lastElement);
	void Clear();
	void* At(int i);
	void* operator[](int i);
	void* GetArray();
	/* Returns number of elements, not size in bytes*/
	size_t Size();
};