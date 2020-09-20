#pragma once

#include <memory>
#include "spdlog/spdlog.h"

class Log
{
public:
	Log() = default;
	~Log() = default;

	static void Init();
	static inline std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

private:
	static std::shared_ptr<spdlog::logger> s_Logger;
};

// LOGGING ENABLED
#define LOG_TRACE(...)   ::Log::GetLogger()->trace(__VA_ARGS__);
#define LOG_INFO(...)    ::Log::GetLogger()->info(__VA_ARGS__);
#define LOG_WARN(...)    ::Log::GetLogger()->warn(__VA_ARGS__);
#define LOG_ERROR(...)   ::Log::GetLogger()->error(__VA_ARGS__);
#define LOG_FATAL(...)   ::Log::GetLogger()->fatal(__VA_ARGS__);

// LOGGING DISABLED
// #define LOG_TRACE   
// #define LOG_INFO   
// #define LOG_WARN 
// #define LOG_ERROR
// #define LOG_FATAL 



