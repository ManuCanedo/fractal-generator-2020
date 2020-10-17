#pragma once

namespace Fractal
{
	class Timer
	{
	public:
		Timer() : m_StartTimepoint(std::chrono::high_resolution_clock::now()) {}
		~Timer() { Stop(); }

		void Stop()
		{
			const auto endTimepoint{ std::chrono::high_resolution_clock::now() };
			const auto start{ std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count() };
			const auto end{ std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count() };

			const double ms{ (end - start) * 0.001 };
			LOG_WARN("Function execution time: {0}(ms)", ms);
		}

	private:
		const std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
	};
}
