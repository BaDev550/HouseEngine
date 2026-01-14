#include "hepch.h"
#include "Logger.h"

MEM::Ref<spdlog::logger> Logger::s_CoreLogger = nullptr;
MEM::Ref<spdlog::logger> Logger::s_ClientLogger = nullptr;