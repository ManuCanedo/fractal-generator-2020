#pragma once

#include "Log.h"

namespace Fractal
{
	class Timer
	{
	public:
		Timer() : m_StartTimepoint(std::chrono::high_resolution_clock::now()) {}
		~Timer() { Stop(); }

		inline void Stop()
		{
			auto endTimepoint{ std::chrono::high_resolution_clock::now() };
			auto start{ std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count() };
			auto end{ std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count() };
			auto duration{ end - start };
			double ms = duration * 0.001;

			LOG_WARN("Function execution time: {0}(ms)", ms);
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
	};
}
