#pragma once

class DynamicArray
{
private:
	size_t sizeOfOneElement;
	size_t meanNumberOfElements;
	size_t realNumberOfElements;
	void* array;
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
	void* operator[](int i);
	/* Returns number of elements, not size in bytes*/
	size_t Size();
};