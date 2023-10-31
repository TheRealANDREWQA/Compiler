#pragma once
#include <stdint.h>

typedef struct {
	void* buffer;
	size_t size;
	size_t capacity;
	size_t element_size;
} ResizableStream;

/*
	Adds a new value to the dynamic array. It will resize the array if the size is equal to the capacity.
*/
void Add(ResizableStream* stream, const void* element);

/*
	Creates a dynamic array with the given initial capacity and the element byte size. If the capacity
	is zero, it will not allocate any memory
*/
ResizableStream CreateStream(size_t capacity, size_t element_size);

/*
	Frees the memory used by the array if a buffer is currently allocated.
*/
void FreeStream(ResizableStream stream);

/*
	Copies the dynamic array contents into a new buffer and returns that new buffer.
*/
ResizableStream CopyStream(ResizableStream other);

/*
	Returns a pointer to an element given its index. Does not do bounds checking.
*/
void* GetElement(ResizableStream stream, size_t index);

/*
	Removes an element from the dynamic array and moves all the elements after it one location back.
	Does not do bounds checking on the index
*/
void Remove(ResizableStream* stream, size_t index);

/*
	Removes an element from the dynamic array by moving the last element in its place. Does not do bounds
	checking on the index.
*/
void RemoveSwapBack(ResizableStream* stream, size_t index);

/*
	Searches linearly through the array for an element by using a byte wise equality. 
	Returns the index of the element if it is found, else -1.
*/
size_t Find(ResizableStream stream, const void* element);

/*
	Resizes the dynamic to a new given capacity. If the capacity is greater than the current capacity,
	all the elements will be copied over. If the new capacity is smaller than the current capacity,
	then the last elements will be trimmed.
*/
void Resize(ResizableStream* stream, size_t new_capacity);

/*
	Grows the array to accomodate element_count new elements if the current capacity does not allow it.
*/
void Reserve(ResizableStream* stream, size_t element_count);

/*
	Modifies an element at a given index. Does not do bounds checking.
*/
void SetElement(ResizableStream stream, size_t index, const void* new_value);