#pragma once
#include <string>

namespace hic200 {

// Mirrors src/read_and_dump2.py: reads a gzipped fragment-pair file,
// keeps cis pairs (same chromosome) within `max_distance` of each other by
// midpoint, maps each fragment's midpoint to a bin from `bin_file`, and
// writes "bin1\tbin2\tscore" rows.
void read_and_dump(const std::string& bin_file, long long max_distance,
                    const std::string& input_gz, const std::string& output_file);

}  // namespace hic200
