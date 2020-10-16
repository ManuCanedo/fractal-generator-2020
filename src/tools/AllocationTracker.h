#pragma once

struct AllocationTracker
{
	int TotalAllocated{ 0 };
	int TotalFreed{ 0 };

	constexpr int CurrentUsage()
	{
		return static_cast<int>(TotalAllocated - TotalFreed);
	}

} s_AllocationTracker;

void* operator new(const size_t size)
{
	s_AllocationTracker.TotalAllocated += static_cast<int>(size);
	return malloc(size);
}

void operator delete(void* memory, const size_t size)
{
	s_AllocationTracker.TotalFreed += static_cast<int>(size);
	free(memory);
}