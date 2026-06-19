#pragma once
#include <string>
#include <vector>

namespace hic200 {

// Splits on runs of whitespace, like Python's str.split() with no arguments.
std::vector<std::string> split_ws(const std::string& s);

// Strips leading/trailing whitespace, like Python's str.strip().
std::string strip(const std::string& s);

}  // namespace hic200
