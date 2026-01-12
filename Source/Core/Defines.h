#pragma once
#include <iostream>

#define CHECKF(x, ...) \
	if ((x)) \
		throw std::runtime_error(__VA_ARGS__);

#ifdef USE_ASSERT
#define ASSERT(x, ...)
#endif