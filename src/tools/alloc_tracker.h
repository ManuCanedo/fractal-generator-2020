#ifndef ALLOC_TRACKER_H
#define ALLOC_TRACKER_H

struct AllocationTracker
{
	int total_alloc{ 0 };
	int total_freed{ 0 };

	constexpr int curr_usage() const
	{
		return static_cast<int>(total_alloc - total_freed);
	}
} s_alloc_tracker;

void* operator new(const size_t size)
{
	s_alloc_tracker.total_alloc += static_cast<int>(size);
	void* sz = malloc(size);
	return sz ? sz : nullptr;
}

void operator delete(void* mem, const size_t size)
{
	s_alloc_tracker.total_freed += static_cast<int>(size);
	free(mem);
}

#endif // ALLOC_TRACKER_H