#pragma once
#include <string>

namespace hic200 {

// Mirrors src/dump_to_sort.py, but sorts and aggregates in memory instead of
// shelling out to the `sort` command. Reads "bin1\tbin2\tscore" rows from
// `pairs_file`, sorts by (bin1, bin2), sums scores for identical pairs, and
// writes "bin1\tbin2\tscore" rows to `output_file`.
//
// If `sorted_output` is non-empty, the sorted (pre-aggregation) rows are
// also written there -- matching the optional intermediate file the
// original CLI could produce via --sorted.
void sort_and_aggregate(const std::string& pairs_file, const std::string& output_file,
                         const std::string& sorted_output = "");

}  // namespace hic200
