#pragma once

struct AllocationTracker
{
	int TotalAllocated{ 0 };
	int TotalFreed{ 0 };

	int CurrentUsage()
	{
		return static_cast<int>(TotalAllocated - TotalFreed);
	}

} s_AllocationTracker;

void* operator new(size_t size)
{
	s_AllocationTracker.TotalAllocated += static_cast<int>(size);
	return malloc(size);
}

void operator delete(void* memory, size_t size)
{
	s_AllocationTracker.TotalFreed += static_cast<int>(size);
	free(memory);
}