#pragma once

#include <memory>
#include <string>

namespace LDPCTests
{
    std::unique_ptr<char> read_binary_file(const std::string& filename, size_t expected_length = 0);
}
