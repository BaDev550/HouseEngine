#pragma once
#include <string>

namespace Platform {

	class FileDialog
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
		static std::string OpenDirectory();
	};

	class DateTime
	{
	public:
		static uint64_t GetCurrentDateTimeU64();
		static std::string GetCurrentDateTimeString();
	};
}