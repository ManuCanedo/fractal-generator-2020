#ifndef TIMER_H
#define TIMER_H

namespace fractal
{
class Timer {
public:
	Timer() : start_timepoint(std::chrono::high_resolution_clock::now())
	{
	}

	~Timer()
	{
		stop();
	}

	void stop()
	{
		const auto end_timepoint = std::chrono::high_resolution_clock::now();
		const auto start =
			std::chrono::time_point_cast<std::chrono::microseconds>(start_timepoint)
				.time_since_epoch()
				.count();
		const auto end =
			std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint)
				.time_since_epoch()
				.count();

		const double ms = (end - start) * 0.001;
		LOG_WARN("Function execution time: {0}(ms)", ms);
	}

private:
	const std::chrono::time_point<std::chrono::high_resolution_clock> start_timepoint;
};
}

#endif // TIMER_H