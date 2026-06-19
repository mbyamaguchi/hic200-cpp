#include "gz_reader.hpp"

#include <zlib.h>

#include <stdexcept>

namespace hic200 {

GzLineReader::GzLineReader(const std::string& path) : gz_(nullptr) {
    gzFile f = gzopen(path.c_str(), "rb");
    if (!f) {
        throw std::runtime_error("cannot open gzip file: " + path);
    }
    gzbuffer(f, 1 << 20);  // 1MB internal buffer for throughput on large files
    gz_ = f;
}

GzLineReader::~GzLineReader() {
    if (gz_) gzclose(static_cast<gzFile>(gz_));
}

bool GzLineReader::getline(std::string& out) {
    out.clear();
    gzFile f = static_cast<gzFile>(gz_);
    char buf[1 << 16];
    bool got_any = false;
    for (;;) {
        char* r = gzgets(f, buf, sizeof(buf));
        if (!r) {
            return got_any;
        }
        got_any = true;
        out += r;
        if (!out.empty() && out.back() == '\n') {
            out.pop_back();
            if (!out.empty() && out.back() == '\r') out.pop_back();
            return true;
        }
        // else: line longer than buf, loop to keep accumulating it.
    }
}

}  // namespace hic200
