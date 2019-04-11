#include "DynamicArray.h"

DynamicArray::DynamicArray(size_t sizeOfOneElement)
	:sizeOfOneElement(sizeOfOneElement)
{
}

DynamicArray::~DynamicArray()
{
}

void DynamicArray::PushBack(void * element)
{
}

void DynamicArray::Resize(size_t newSize)
{

}

void * DynamicArray::operator[](int i)
{
	return (char*)array + sizeOfOneElement * i;
}
