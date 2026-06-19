#include <iostream>
#include <string>
#include <vector>

#include "aggregate.hpp"

namespace {

void print_usage() {
    std::cerr << "Usage: dump_to_sort [--sorted SORTED_PATH] [--sortmemory SIZE] "
                 "[--tmpdir DIR] TMPPAIRS OUTPUT\n"
                 "  --sortmemory and --tmpdir are accepted for compatibility with\n"
                 "  the original Python CLI but are unused: this port sorts and\n"
                 "  aggregates in memory instead of shelling out to `sort`.\n";
}

}  // namespace

int main(int argc, char** argv) {
    std::string sorted_path;
    std::vector<std::string> positional;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--sorted" && i + 1 < argc) {
            sorted_path = argv[++i];
        } else if (arg == "--sortmemory" && i + 1 < argc) {
            ++i;  // accepted, ignored
        } else if (arg == "--tmpdir" && i + 1 < argc) {
            ++i;  // accepted, ignored
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
        hic200::sort_and_aggregate(positional[0], positional[1], sorted_path);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
