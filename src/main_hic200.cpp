#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "aggregate.hpp"
#include "bin_def.hpp"
#include "read_and_dump.hpp"

namespace fs = std::filesystem;

namespace {

void print_usage() { std::cerr << "Usage: hic200 [--tmpdir DIR] SITE_FILE INPUTPATH OUTPUTPATH\n"; }

}  // namespace

int main(int argc, char** argv) {
    std::string tmpdir = "./tmp";
    std::vector<std::string> positional;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--tmpdir" && i + 1 < argc) {
            tmpdir = argv[++i];
        } else if (arg == "-h" || arg == "--help") {
            print_usage();
            return 0;
        } else {
            positional.push_back(arg);
        }
    }

    if (positional.size() != 3) {
        print_usage();
        return 1;
    }

    const std::string& site_file = positional[0];
    const std::string& inputpath = positional[1];
    const std::string& outputpath = positional[2];

    if (!fs::is_directory(tmpdir)) {
        std::cout << "directory " << tmpdir << " does not exist.\n";
        std::cout << "proceed to make " << tmpdir << "? [y/N]";
        std::string ans;
        std::getline(std::cin, ans);
        if (ans != "y" && ans != "Y") {
            std::cout << "exit.\n";
            return 0;
        }
        fs::create_directories(tmpdir);
    }

    const std::string bindefpath = tmpdir + "/tmp_bindef.txt";
    const std::string tmppairpath = tmpdir + "/tmp_pair.txt";
    const std::string sortedpath = tmpdir + "/tmp_sorted.txt";

    try {
        hic200::make_bin_def(200, {"I", "II", "III"}, site_file, bindefpath);
        hic200::read_and_dump(bindefpath, 1'000'000, inputpath, tmppairpath);
        hic200::sort_and_aggregate(tmppairpath, outputpath, sortedpath);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
