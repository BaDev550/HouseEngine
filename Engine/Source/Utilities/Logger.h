#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "Memory.h"
#include <format>
#include <iostream>

#define CHECK_RENDERER_LOGGER if(Logger::GetRendererLogger() != nullptr)
#define CHECK_LOGGER if(Logger::GetCoreLogger() != nullptr)
#define CHECK_APP_LOGGER if(Logger::GetClientLogger() != nullptr)

#define LOG_RENDERER_TRANCE(...)   CHECK_RENDERER_LOGGER{Logger::GetRendererLogger()->trace(__VA_ARGS__);}
#define LOG_RENDERER_INFO(...)     CHECK_RENDERER_LOGGER{Logger::GetRendererLogger()->info(__VA_ARGS__);}
#define LOG_RENDERER_WARN(...)     CHECK_RENDERER_LOGGER{Logger::GetRendererLogger()->warn(__VA_ARGS__);}
#define LOG_RENDERER_ERROR(...)    CHECK_RENDERER_LOGGER{Logger::GetRendererLogger()->error(__VA_ARGS__);}
#define LOG_RENDERER_CRITICAL(...) CHECK_RENDERER_LOGGER{Logger::GetRendererLogger()->critical(__VA_ARGS__);}

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

class Logger {
public:
#define LOG_PATTERN "[%H:%M:%S] [%n] %^[%l]: %v%$"
    static void init() {
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_color_mode(spdlog::color_mode::always);
        //spdlog::set_pattern("%^[%n][%l] %T: %v%$");

        s_CoreLogger = std::make_shared<spdlog::logger>("CORE", consoleSink);
        spdlog::register_logger(s_CoreLogger);
        s_CoreLogger->set_pattern(LOG_PATTERN);
        s_CoreLogger->set_level(spdlog::level::trace);
        
        s_ClientLogger = std::make_shared<spdlog::logger>("APP", consoleSink);
        spdlog::register_logger(s_ClientLogger);
        s_CoreLogger->set_pattern(LOG_PATTERN);
        s_ClientLogger->set_level(spdlog::level::trace);

        s_RenderLogger = std::make_shared<spdlog::logger>("RENDER", consoleSink);
        spdlog::register_logger(s_RenderLogger);
        s_CoreLogger->set_pattern(LOG_PATTERN);
        s_RenderLogger->set_level(spdlog::level::debug);
    }
    static void Destroy() {
        s_CoreLogger = nullptr;
        s_RenderLogger = nullptr;
        s_ClientLogger = nullptr;
    }

    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    inline static std::shared_ptr<spdlog::logger>& GetRendererLogger() { return s_RenderLogger; }
    inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_RenderLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};