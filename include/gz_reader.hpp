#pragma once
#include <string>

namespace hic200 {

// Thin line-oriented wrapper around zlib's gzFile. Kept opaque (void*) here
// so callers don't need <zlib.h>.
class GzLineReader {
public:
    explicit GzLineReader(const std::string& path);
    ~GzLineReader();

    GzLineReader(const GzLineReader&) = delete;
    GzLineReader& operator=(const GzLineReader&) = delete;

    // Reads the next line (without trailing \n or \r\n) into `out`.
    // Returns false once there is nothing left to read.
    bool getline(std::string& out);

private:
    void* gz_;
};

}  // namespace hic200
