#include "bin_def.hpp"

#include <algorithm>
#include <fstream>
#include <map>
#include <set>
#include <stdexcept>

#include "common.hpp"

namespace hic200 {

namespace {

std::map<std::string, long long> read_chromosome_length(
    const std::string& site_file, const std::set<std::string>& target_chroms) {
    std::ifstream f(site_file);
    if (!f) {
        throw std::runtime_error("cannot open site file: " + site_file);
    }

    std::map<std::string, long long> chrom_lengths;

    std::string line;
    while (std::getline(f, line)) {
        line = strip(line);
        if (line.empty() || line[0] == '#') continue;

        auto fields = split_ws(line);
        if (fields.size() < 5) continue;

        const std::string& chrom = fields[1];
        if (target_chroms.find(chrom) == target_chroms.end()) continue;

        long long position = std::stoll(fields[2]);
        long long length_after = std::stoll(fields[4]);
        long long chrom_end = position + length_after;

        auto it = chrom_lengths.find(chrom);
        if (it == chrom_lengths.end()) {
            chrom_lengths.emplace(chrom, chrom_end);
        } else if (chrom_end > it->second) {
            it->second = chrom_end;
        }
    }

    return chrom_lengths;
}

}  // namespace

void make_bin_def(int bin_size, const std::vector<std::string>& chroms,
                   const std::string& site_file, const std::string& output_bin_file) {
    std::set<std::string> target_set(chroms.begin(), chroms.end());
    auto chrom_lengths = read_chromosome_length(site_file, target_set);

    std::string missing;
    for (const auto& chrom : chroms) {
        if (chrom_lengths.find(chrom) == chrom_lengths.end()) {
            if (!missing.empty()) missing += ", ";
            missing += chrom;
        }
    }
    if (!missing.empty()) {
        throw std::runtime_error("Chromosome(s) not found in site file: " + missing);
    }

    std::ofstream out(output_bin_file);
    if (!out) {
        throw std::runtime_error("cannot open output bin file for writing: " + output_bin_file);
    }

    out << "bin\tchr\tstart\tend\n";

    long long bin_id = 1;
    for (const auto& chrom : chroms) {
        long long chrom_length = chrom_lengths[chrom];
        for (long long start = 0; start < chrom_length; start += bin_size) {
            long long end = std::min<long long>(start + bin_size - 1, chrom_length - 1);
            out << bin_id << '\t' << chrom << '\t' << start << '\t' << end << '\n';
            ++bin_id;
        }
    }
}

}  // namespace hic200
