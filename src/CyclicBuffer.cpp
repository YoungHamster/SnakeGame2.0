#include "CyclicBuffer.h"

CyclicBuffer::CyclicBuffer(size_t sizeOfOneElement, int numberOfElements)
	:sizeOfOneElement(sizeOfOneElement), numberOfElements(numberOfElements)
{
	buffer = new char[sizeOfOneElement * numberOfElements];
	for (int i = 0; i < sizeOfOneElement * numberOfElements; i++)
	{
		*((char*)buffer + i) = 0;
	}
}

CyclicBuffer::~CyclicBuffer()
{
	if (buffer)
	{
		delete[] buffer;
	}
}

void * CyclicBuffer::GetNextElement()
{
	if (currentElement == numberOfElements - 1)
	{
		currentElement = 0;
		return buffer;
	}
	else
	{
		currentElement += 1;
	}
	/* vs compiler doesn't allow to do pointer arithmetic with void* so i have to cast it char* */
	return (char*)buffer + currentElement * sizeOfOneElement;
}

void * CyclicBuffer::GetElementByOffset(int offset)
{
	int id = (currentElement + offset) % numberOfElements;
	if (id < 0)
	{
		id = numberOfElements + id;
	}
	return (char*)buffer + id * sizeOfOneElement;
}
