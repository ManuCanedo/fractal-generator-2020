#pragma once

#include <memory>

struct AllocationTracker
{
	uint32_t TotalAllocated{ 0 };
	uint32_t TotalFreed{ 0 };

	uint32_t CurrentUsage()
	{
		return TotalAllocated - TotalFreed;
	}
};

static AllocationTracker s_AllocationTracker;

void* operator new(size_t size)
{
	s_AllocationTracker.TotalAllocated += size;

	return malloc(size);
}

void operator delete(void* memory, size_t size)
{
	s_AllocationTracker.TotalFreed += size;

	free(memory);
}