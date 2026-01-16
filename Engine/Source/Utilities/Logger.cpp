#include "hepch.h"
#include "Logger.h"

std::shared_ptr<spdlog::logger> Logger::s_CoreLogger = nullptr;
std::shared_ptr<spdlog::logger> Logger::s_RenderLogger = nullptr;
std::shared_ptr<spdlog::logger> Logger::s_ClientLogger = nullptr;