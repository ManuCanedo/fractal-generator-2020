#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

namespace fractal
{
class Logger {
public:
	static void Init();

	static std::shared_ptr<spdlog::logger> &Get()
	{
		return s_logger;
	}

private:
	static std::shared_ptr<spdlog::logger> s_logger;
};
}

// LOGGING ENABLED
#define LOG_TRACE(...) ::fractal::Logger::Get()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::fractal::Logger::Get()->info(__VA_ARGS__)
#define LOG_WARN(...) ::fractal::Logger::Get()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::fractal::Logger::Get()->error(__VA_ARGS__)
#define LOG_FATAL(...) ::fractal::Logger::Get()->fatal(__VA_ARGS__)

// LOGGING DISABLED
// #define LOG_TRACE
// #define LOG_INFO
// #define LOG_WARN
// #define LOG_ERROR
// #define LOG_FATAL

#endif // LOGGER_H