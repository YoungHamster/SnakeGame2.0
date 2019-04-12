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
		delete[] array;
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
	memcpy((char*)array + (meanNumberOfElements + 1) * sizeOfOneElement, element, sizeOfOneElement);
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
	meanNumberOfElements -= lastElement - firstElement;
	memcpy((char*)array + firstElement * sizeOfOneElement,
		(char*)array + (lastElement + 1) * sizeOfOneElement,
		sizeOfOneElement * (meanNumberOfElements - lastElement));
}

void DynamicArray::Clear()
{
	meanNumberOfElements = 0;
	realNumberOfElements = 0;
	delete[] array;
}

void * DynamicArray::operator[](int i)
{
	return (char*)array + sizeOfOneElement * i;
}

size_t DynamicArray::Size()
{
	return meanNumberOfElements;
}
