#pragma once

class CyclicBuffer
{
private:
	size_t sizeOfOneElement;
	void* buffer;
	int currentElement = 0;
	int numberOfElements;
public:
	CyclicBuffer(size_t sizeOfOneElement, int numberOfElements);
	~CyclicBuffer();
	void* GetNextElement();
	/* offset is relative to currentElement */
	void* GetElementByOffset(int offset);
};