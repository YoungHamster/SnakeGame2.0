#include "DynamicArray.h"

#include <stdlib.h>
#include <cstring>

DynamicArray::DynamicArray(size_t sizeOfOneElement)
	:sizeOfOneElement(sizeOfOneElement){}

DynamicArray::DynamicArray(size_t sizeOfOneElement, size_t numberOfElements)
	: sizeOfOneElement(sizeOfOneElement), meanNumberOfElements(numberOfElements), realNumberOfElements(numberOfElements)
{
	array = malloc(numberOfElements * sizeOfOneElement);
}

DynamicArray::~DynamicArray()
{
	if (array)
	{
		free(array);
		array = nullptr;
	}
}

void DynamicArray::PushBack(void * element)
{
	if (meanNumberOfElements == realNumberOfElements)
	{
		array = realloc(array, (meanNumberOfElements + 1) * sizeOfOneElement);
		realNumberOfElements += 1;
	}
	meanNumberOfElements += 1;
	memcpy((char*)array + (meanNumberOfElements - 1) * sizeOfOneElement, element, sizeOfOneElement);
}

void DynamicArray::PopBack()
{
	meanNumberOfElements -= 1;
}

void DynamicArray::Resize(size_t newNumberOfElements)
{
	if (newNumberOfElements > realNumberOfElements)
	{
		array = realloc(array, newNumberOfElements * sizeOfOneElement);
		realNumberOfElements = newNumberOfElements;
	}
	meanNumberOfElements = newNumberOfElements;
}

void DynamicArray::Delete(int where)
{
	meanNumberOfElements -= 1;
	memcpy((char*)array + where * sizeOfOneElement,
		(char*)array + (where + 1) * sizeOfOneElement,
		sizeOfOneElement * (meanNumberOfElements - where));
}

void DynamicArray::Delete(int firstElement, int lastElement)
{
	memcpy((char*)array + (firstElement - 1) * sizeOfOneElement,
		(char*)array + lastElement * sizeOfOneElement,
		sizeOfOneElement * (meanNumberOfElements - lastElement - 1));
	meanNumberOfElements -= lastElement - firstElement;
}

void DynamicArray::Clear()
{
	meanNumberOfElements = 0;
	realNumberOfElements = 0;
	free(array);
	array = nullptr;
}

void * DynamicArray::At(int i)
{
	return (char*)array + sizeOfOneElement * i;
}

void * DynamicArray::operator[](int i)
{
	return At(i);
}

void * DynamicArray::GetArray()
{
	return array;
}

size_t DynamicArray::Size()
{
	return meanNumberOfElements;
}
