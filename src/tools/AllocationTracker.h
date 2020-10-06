#pragma once

struct AllocationTracker
{
	int TotalAllocated{ 0 };
	int TotalFreed{ 0 };

	int CurrentUsage()
	{
		return TotalAllocated - TotalFreed;
	}
} s_AllocationTracker;

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