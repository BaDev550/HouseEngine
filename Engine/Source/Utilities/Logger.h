#pragma once

#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include "Memory.h"
#include <format>

#if defined(_MSC_VER)
#define DEBUG_BREAK __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#define DEBUG_BREAK __builtin_trap()
#else
#include <csignal>
#define DEBUG_BREAK raise(SIGTRAP)
#endif
#define CHECK_LOGGER if(Logger::GetCoreLogger() != nullptr)
#define CHECK_APP_LOGGER if(Logger::GetClientLogger() != nullptr)

#define LOG_CORE_TRACE(...)    CHECK_LOGGER{Logger::GetCoreLogger()->trace(__VA_ARGS__); }
#define LOG_CORE_INFO(...)     CHECK_LOGGER{Logger::GetCoreLogger()->info(__VA_ARGS__); }
#define LOG_CORE_WARN(...)     CHECK_LOGGER{Logger::GetCoreLogger()->warn(__VA_ARGS__); }
#define LOG_CORE_ERROR(...)    CHECK_LOGGER{Logger::GetCoreLogger()->error(__VA_ARGS__); } 
#define LOG_CORE_CRITICAL(...) CHECK_LOGGER{Logger::GetCoreLogger()->critical(__VA_ARGS__); }

#define APP_LOG_TRACE(...)    CHECK_APP_LOGGER{Logger::GetClientLogger()->trace(__VA_ARGS__); }
#define APP_LOG_INFO(...)     CHECK_APP_LOGGER{Logger::GetClientLogger()->info(__VA_ARGS__); }
#define APP_LOG_WARN(...)     CHECK_APP_LOGGER{Logger::GetClientLogger()->warn(__VA_ARGS__); }
#define APP_LOG_ERROR(...)    CHECK_APP_LOGGER{Logger::GetClientLogger()->error(__VA_ARGS__); } 
#define APP_LOG_CRITICAL(...) CHECK_APP_LOGGER{Logger::GetClientLogger()->critical(__VA_ARGS__); }
#define ASSERT(x, ...) \
    if (!(x)) { \
        LOG_CRITICAL("ASSERTION FAILED: ({0})", #x); \
        LOG_CRITICAL(__VA_ARGS__); \
        DEBUG_BREAK; \
    }

#define ASSERT_NOMSG(x) \
    if (!(x)) { \
        LOG_CRITICAL("ASSERTION FAILED: ({0})", #x); \
        DEBUG_BREAK; \
    }

class Logger {
public:
    static void init() {
        auto consoleSink = MEM::Ref<spdlog::sinks::stdout_color_sink_mt>::Create();
        spdlog::set_pattern("%^[%n][%l] %T: %v%$");

        s_CoreLogger = std::make_shared<spdlog::logger>("CORE", consoleSink);
        spdlog::register_logger(s_CoreLogger);
        s_CoreLogger->set_level(spdlog::level::trace);

        s_ClientLogger = std::make_shared<spdlog::logger>("APP", consoleSink);
        spdlog::register_logger(s_ClientLogger);
        s_ClientLogger->set_level(spdlog::level::trace);
    }

    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};