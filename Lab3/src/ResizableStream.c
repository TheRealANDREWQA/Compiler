#include "ResizableStream.h"
#include <stdlib.h>
#include <string.h>

#define RESIZE_FACTOR 1.5f

/*
	Offsets a pointer given a byte offset
*/
static void* OffsetPointer(const void* pointer, size_t offset) {
	uintptr_t ptr = (uintptr_t)pointer;
	return (void*)(ptr + offset);
}

size_t ResizeCapacity(size_t capacity) {
	return (size_t)(RESIZE_FACTOR * (float)capacity + 1);
}

void Add(ResizableStream* stream, const void* element) {
	if (stream->size == stream->capacity) {
		stream->capacity = ResizeCapacity(stream->capacity);
		if (stream->buffer != NULL) {
			stream->buffer = realloc(stream->buffer, stream->capacity * stream->element_size);
		}
		else {
			stream->buffer = malloc(stream->element_size * stream->capacity);
		}
	}

	SetElement(*stream, stream->size, element);
	stream->size++;
}

ResizableStream CreateStream(size_t capacity, size_t element_size) {
	if (capacity > 0) {
		void* buffer = malloc(element_size * capacity);
		return (ResizableStream){ buffer, 0, capacity, element_size };
	}
	else {
		return (ResizableStream){ NULL, 0, 0, element_size };
	}
}

void FreeStream(ResizableStream stream) {
	if (stream.buffer != NULL) {
		free(stream.buffer);
	}
}

ResizableStream CopyStream(ResizableStream other)
{
	ResizableStream copy = CreateStream(other.size, other.element_size);
	memcpy(copy.buffer, other.buffer, other.element_size * other.size);
	copy.size = other.size;

	return copy;
}

void* GetElement(ResizableStream stream, size_t index) {
	return OffsetPointer(stream.buffer, index * stream.element_size);
}

void Remove(ResizableStream* stream, size_t index) {
	memmove(OffsetPointer(stream->buffer, stream->element_size * (index + 1)), OffsetPointer(stream->buffer, stream->element_size * index), stream->element_size * (stream->size - index));
	stream->size--;
}

void RemoveSwapBack(ResizableStream* stream, size_t index) {
	stream->size--;
	memcpy(OffsetPointer(stream->buffer, stream->element_size * index), OffsetPointer(stream->buffer, stream->element_size * stream->size), stream->element_size);
}

size_t Find(ResizableStream stream, const void* element) {
	for (size_t index = 0; index < stream.size; index++) {
		if (memcmp(OffsetPointer(stream.buffer, stream.element_size * index), element, stream.element_size) == 0) {
			return index;
		}
	}
	return -1;
}

void Resize(ResizableStream* stream, size_t new_capacity) {
	stream->capacity = new_capacity;
	if (stream->buffer != NULL) {
		stream->buffer = realloc(stream->buffer, stream->capacity * stream->element_size);
	}
	else {
		stream->buffer = malloc(stream->element_size * stream->capacity);
	}
}

void Reserve(ResizableStream* stream, size_t element_count)
{
	if (stream->size + element_count > stream->capacity) {
		Resize(stream, ResizeCapacity(stream->capacity));
	}
}

void SetElement(ResizableStream stream, size_t index, const void* new_value) {
	void* element = GetElement(stream, index);
	memcpy(element, new_value, stream.element_size);
}