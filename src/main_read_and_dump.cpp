#include <iostream>
#include <string>
#include <vector>

#include "read_and_dump.hpp"

namespace {

void print_usage() {
    std::cerr << "Usage: read_and_dump2 --bin BIN_FILE [--max-distance N] "
                 "INPUT_GZ OUTPUT_FILE\n";
}

}  // namespace

int main(int argc, char** argv) {
    std::string bin_file;
    long long max_distance = 1'000'000;
    std::vector<std::string> positional;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--bin" && i + 1 < argc) {
            bin_file = argv[++i];
        } else if (arg == "--max-distance" && i + 1 < argc) {
            max_distance = std::stoll(argv[++i]);
        } else if (arg == "-h" || arg == "--help") {
            print_usage();
            return 0;
        } else {
            positional.push_back(arg);
        }
    }

    if (bin_file.empty() || positional.size() != 2) {
        print_usage();
        return 1;
    }

    try {
        hic200::read_and_dump(bin_file, max_distance, positional[0], positional[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
