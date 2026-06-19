#include "read_and_dump.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>

#include "common.hpp"
#include "gz_reader.hpp"

namespace hic200 {

namespace {

struct ChromMeta {
    long long first_bin;
    long long last_end;
};

// Mirrors load_bin_metadata() in src/read_and_dump2.py: for each chromosome,
// records the id of its first bin and the end coordinate of its last bin.
std::map<std::string, ChromMeta> load_bin_metadata(const std::string& bin_file) {
    std::ifstream f(bin_file);
    if (!f) {
        throw std::runtime_error("cannot open bin file: " + bin_file);
    }

    std::map<std::string, ChromMeta> chr_meta;

    std::string line;
    std::getline(f, line);  // skip header

    while (std::getline(f, line)) {
        std::string s = strip(line);
        if (s.empty()) continue;

        auto fields = split_ws(s);
        if (fields.size() < 4) continue;

        long long bin_id = std::stoll(fields[0]);
        const std::string& chrom = fields[1];
        long long end = std::stoll(fields[3]);

        auto it = chr_meta.find(chrom);
        if (it == chr_meta.end()) {
            chr_meta.emplace(chrom, ChromMeta{bin_id, end});
        } else if (end > it->second.last_end) {
            it->second.last_end = end;
        }
    }

    return chr_meta;
}

inline long long midpoint(long long start, long long end) { return (start + end) / 2; }

// Faithfully mirrors a quirk of midpoint_to_bin() in src/read_and_dump2.py:
// the local bin index is always computed against a hardcoded 200bp bin
// size, regardless of the --bin-size actually used to build the bin file.
// The hic200 orchestrator always builds 200bp bins, so this never bites
// there; it only matters if read_and_dump2 is run standalone against a bin
// file built with a different bin size.
constexpr long long kHardcodedBinSize = 200;

// Returns -1 (the None sentinel) when the chromosome is absent from the bin
// file, or when `mid` falls beyond the chromosome's last bin.
long long midpoint_to_bin(const std::string& chrom, long long mid,
                           const std::map<std::string, ChromMeta>& chr_meta) {
    auto it = chr_meta.find(chrom);
    if (it == chr_meta.end()) return -1;
    if (mid > it->second.last_end) return -1;

    long long local_index = mid / kHardcodedBinSize;
    return it->second.first_bin + local_index;
}

}  // namespace

void read_and_dump(const std::string& bin_file, long long max_distance,
                    const std::string& input_gz, const std::string& output_file) {
    auto chr_meta = load_bin_metadata(bin_file);

    GzLineReader fin(input_gz);
    std::ofstream fout(output_file);
    if (!fout) {
        throw std::runtime_error("cannot open output file for writing: " + output_file);
    }

    fout << "bin1\tbin2\tscore\n";

    std::string line;
    fin.getline(line);  // skip header

    long long skipped_invalid = 0;

    while (fin.getline(line)) {
        std::string s = strip(line);
        if (s.empty()) continue;

        auto fields = split_ws(s);
        if (fields.size() < 9) continue;

        const std::string& chrom1 = fields[0];
        long long start1 = std::stoll(fields[1]);
        long long end1 = std::stoll(fields[2]);

        const std::string& chrom2 = fields[4];
        long long start2 = std::stoll(fields[5]);
        long long end2 = std::stoll(fields[6]);

        const std::string& score = fields[8];

        if (chrom1 != chrom2) continue;

        long long mid1 = midpoint(start1, end1);
        long long mid2 = midpoint(start2, end2);

        if (std::llabs(mid1 - mid2) > max_distance) continue;

        long long bin1 = midpoint_to_bin(chrom1, mid1, chr_meta);
        long long bin2 = midpoint_to_bin(chrom2, mid2, chr_meta);

        // The original Python swaps bin1/bin2 when either is None and then
        // writes the result regardless, which can emit a literal "None"
        // token that breaks the downstream numeric sort/aggregate step. This
        // port skips such pairs instead (see README "Known differences").
        // For well-formed input -- every chromosome covered by the bin
        // file, no midpoint past the chromosome end -- this never triggers,
        // so output is identical to the original.
        if (bin1 < 0 || bin2 < 0) {
            ++skipped_invalid;
            continue;
        }

        fout << bin1 << '\t' << bin2 << '\t' << score << '\n';
    }

    if (skipped_invalid > 0) {
        std::cerr << "[read_and_dump] skipped " << skipped_invalid
                  << " pair(s) with no valid bin\n";
    }
}

}  // namespace hic200
