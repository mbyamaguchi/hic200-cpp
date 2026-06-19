# hic200-cpp

A C++17 rewrite of [hic-200](https://github.com/mbyamaguchi/hic-200) (`hic200.py`), which bins Hi-C
fragment-pair data into a contact map. Same pipeline, three stages:

1. **make_bin_def2** — build a bin-definition table from a restriction-enzyme
   site file.
2. **read_and_dump2** — bin cis fragment pairs (same chromosome, within
   `--max-distance` by fragment midpoint) from a gzipped fragment-pair file.
3. **dump_to_sort** — sort the binned pairs and sum scores for identical
   `(bin1, bin2)` pairs.

`hic200` is the orchestrator that runs all three stages in sequence, just
like `hic200.py`.

Validated against the original Python implementation on real data (see
"Validation" below): byte-for-byte identical output, ~2x faster wall time.

## Build

Requires a C++17 compiler, CMake, and zlib (dev headers).

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Binaries land in `build/`: `hic200`, `make_bin_def2`, `read_and_dump2`,
`dump_to_sort`.

## Usage

Equivalent to the original's "Simple usage":

```sh
./build/hic200 --tmpdir ./tmp SITE_FILE INPUTPATH OUTPUTPATH
```

- `SITE_FILE`: restriction enzyme site file.
- `INPUTPATH`: gzipped fragment-pair count file.
- `OUTPUTPATH`: final binned/aggregated output (`bin\tbin2\tscore`).
- `--tmpdir`: scratch directory for intermediate files (default `./tmp`).
  Uses 200bp bins over chromosomes `I,II,III`, max cis distance 1,000,000bp —
  same hardcoded defaults as `hic200.py`.

### Step by step

Mirrors the original's step-by-step tools:

```sh
./build/make_bin_def2 --bin-size 200 --chroms I,II,III SITE_FILE BIN_FILE

./build/read_and_dump2 --bin BIN_FILE --max-distance 1000000 INPUT_GZ TMP_PAIRS

./build/dump_to_sort TMP_PAIRS OUTPUT [--sorted SORTED_PATH]
```

## Known differences from the original Python

- **Invalid bins are skipped, not propagated as `"None"`.** In
  `read_and_dump2.py`, a fragment whose midpoint has no valid bin (wrong
  chromosome, or past the last bin) produces a Python `None`, which gets
  swapped into one column and written out literally as the string `"None"`.
  That token then breaks the downstream numeric sort/aggregate (GNU `sort`
  treats it as 0; Python's `int("None")` raises and crashes the run). This
  port skips such pairs outright (with a count on stderr) instead. For input
  where every chromosome in the fragment-pair file is covered by the bin
  file and no midpoint falls past a chromosome's end — true for all data
  this pipeline has actually been run on — this code path never triggers, so
  output is unaffected.
- **Hardcoded 200bp re-binning constant, carried over as-is.**
  `midpoint_to_bin()` in the original always divides by a literal `200` to
  get the local bin index, regardless of the `--bin-size` used to build the
  bin file. The `hic200` orchestrator always builds 200bp bins, so this is
  invisible there. It's preserved here unchanged for parity; it only matters
  if you run `read_and_dump2` standalone against a bin file built with
  `--bin-size != 200`.
- **No external `sort` dependency.** `dump_to_sort.py` shells out to GNU
  `sort -k1,1n -k2,2n`. This port sorts the same `(bin1, bin2)` keys with
  `std::sort` in memory and aggregates in the same pass. `--sortmemory` and
  `--tmpdir` are accepted by the `dump_to_sort` binary for CLI compatibility
  but are unused.

## Validation

Checked against `hic-200`'s `hic200.py` using real data from the sibling
`hic-boundary` project (`MboI-Hinf1-MluCl_sites.txt` as the site file,
`wt_HiC_Double-MHM_Bio2_NovaSeq_fragment_pair.txt.gz` as input):

- A 1,000,000-line sample: output byte-identical (md5-matched) between the
  Python and C++ runs.
- The full ~42.4M-line file: output byte-identical to both the Python run
  and the pre-existing `output_NovaSeq.txt` reference (22,057,054 rows,
  md5-matched). Wall time: 25s for this C++ pipeline (single-threaded) vs.
  51s for `hic200.py` (which parallelizes via GNU `sort`).
