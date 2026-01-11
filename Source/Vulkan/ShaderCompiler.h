#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>

namespace pipeline::utils {
    std::vector<char> CompileShaderFileToSpirv(const std::filesystem::path& path, bool optimize = true);
}

