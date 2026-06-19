#pragma once
#include <string>
#include <vector>

namespace hic200 {

// Builds a bin-definition file for `chroms`, mirroring
// utils/make_bin_def2.py from the original hic-200 Python tool.
//
// `site_file` is a restriction-enzyme site table with columns
// "#number chr position length_before length_after"; a chromosome's
// length is inferred as max(position + length_after) over its rows.
// Bin ids are assigned globally, continuing across chromosomes in the
// order given by `chroms`.
//
// Throws std::runtime_error if any chromosome in `chroms` has no rows in
// `site_file`.
void make_bin_def(int bin_size,
                   const std::vector<std::string>& chroms,
                   const std::string& site_file,
                   const std::string& output_bin_file);

}  // namespace hic200
