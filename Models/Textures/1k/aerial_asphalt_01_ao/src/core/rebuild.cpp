#include "core/rebuild.hpp"
#include "data/texture_parts.hpp"
#include "io/file_write.hpp"

#include <cstdint>
#include <vector>

namespace core
{
bool RebuildToFile(const std::filesystem::path& outPath)
{
    std::vector<std::uint8_t> bytes;
    texture_data::append_all_parts(bytes);
    return io::WriteBinaryFile(outPath, bytes);
}
}
