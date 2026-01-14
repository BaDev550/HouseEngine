#pragma once
#include <iostream>
#include "Logger.h"

#define USE_CUSTOM_LOGGER
#ifdef USE_CUSTOM_LOGGER
#define CHECKF(x, ...) \
	if ((x)) \
		LOG_CORE_CRITICAL(__VA_ARGS__)
#else
#define CHECKF(x, ...) \
	if ((x)) \
		throw std::runtime_error(__VA_ARGS__);
#endif

#ifdef USE_ASSERT
#define ASSERT(x, ...)
#endif