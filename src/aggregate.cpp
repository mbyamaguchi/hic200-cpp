#include "aggregate.hpp"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "common.hpp"

namespace hic200 {

namespace {

struct PairRecord {
    long long bin1;
    long long bin2;
    long long score;
};

}  // namespace

void sort_and_aggregate(const std::string& pairs_file, const std::string& output_file,
                         const std::string& sorted_output) {
    std::ifstream fin(pairs_file);
    if (!fin) {
        throw std::runtime_error("cannot open pairs file: " + pairs_file);
    }

    std::vector<PairRecord> records;

    std::string line;
    std::getline(fin, line);  // skip header ("bin1\tbin2\tscore")

    while (std::getline(fin, line)) {
        std::string s = strip(line);
        if (s.empty()) continue;

        auto fields = split_ws(s);
        if (fields.size() < 3) continue;

        records.push_back(PairRecord{std::stoll(fields[0]), std::stoll(fields[1]),
                                      std::stoll(fields[2])});
    }

    std::sort(records.begin(), records.end(), [](const PairRecord& a, const PairRecord& b) {
        if (a.bin1 != b.bin1) return a.bin1 < b.bin1;
        return a.bin2 < b.bin2;
    });

    if (!sorted_output.empty()) {
        std::ofstream fsorted(sorted_output);
        if (!fsorted) {
            throw std::runtime_error("cannot open sorted output file: " + sorted_output);
        }
        fsorted << "bin1\tbin2\tscore\n";
        for (const auto& r : records) {
            fsorted << r.bin1 << '\t' << r.bin2 << '\t' << r.score << '\n';
        }
    }

    std::ofstream fout(output_file);
    if (!fout) {
        throw std::runtime_error("cannot open output file: " + output_file);
    }
    fout << "bin1\tbin2\tscore\n";

    bool have_prev = false;
    long long prev_b1 = 0, prev_b2 = 0, total = 0;

    for (const auto& r : records) {
        if (!have_prev) {
            prev_b1 = r.bin1;
            prev_b2 = r.bin2;
            total = r.score;
            have_prev = true;
        } else if (r.bin1 == prev_b1 && r.bin2 == prev_b2) {
            total += r.score;
        } else {
            fout << prev_b1 << '\t' << prev_b2 << '\t' << total << '\n';
            prev_b1 = r.bin1;
            prev_b2 = r.bin2;
            total = r.score;
        }
    }

    if (have_prev) {
        fout << prev_b1 << '\t' << prev_b2 << '\t' << total << '\n';
    }
}

}  // namespace hic200
