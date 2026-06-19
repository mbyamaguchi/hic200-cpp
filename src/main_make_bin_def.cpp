#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "bin_def.hpp"

namespace {

std::vector<std::string> split_comma(const std::string& s) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ',')) out.push_back(item);
    return out;
}

void print_usage() {
    std::cerr << "Usage: make_bin_def2 [--bin-size N] [--chroms I,II,III] "
                 "SITE_FILE OUTPUT_BIN_FILE\n";
}

}  // namespace

int main(int argc, char** argv) {
    int bin_size = 200;
    std::string chroms = "I,II,III";
    std::vector<std::string> positional;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--bin-size" && i + 1 < argc) {
            bin_size = std::stoi(argv[++i]);
        } else if (arg == "--chroms" && i + 1 < argc) {
            chroms = argv[++i];
        } else if (arg == "-h" || arg == "--help") {
            print_usage();
            return 0;
        } else {
            positional.push_back(arg);
        }
    }

    if (positional.size() != 2) {
        print_usage();
        return 1;
    }

    try {
        hic200::make_bin_def(bin_size, split_comma(chroms), positional[0], positional[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
