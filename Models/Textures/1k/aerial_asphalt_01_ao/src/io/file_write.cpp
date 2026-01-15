#include "io/file_write.hpp"

#include <fstream>

namespace io
{
bool WriteBinaryFile(const std::filesystem::path& path, const std::vector<std::uint8_t>& bytes)
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream f(path, std::ios::binary);
    if (!f)
        return false;
    if (!bytes.empty())
        f.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    return true;
}
}
