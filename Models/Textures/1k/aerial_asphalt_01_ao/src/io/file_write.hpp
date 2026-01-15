#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

namespace io
{
bool WriteBinaryFile(const std::filesystem::path& path, const std::vector<std::uint8_t>& bytes);
}
