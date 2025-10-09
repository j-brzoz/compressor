# Huffman Compressor

**Huffman Compressor** is a C implementation of a file compression and decompression utility using Huffman coding - a lossless data compression algorithm that assigns variable-length codes to characters based on their frequency of occurrence.

---

## Table of Contents

- [Huffman Compressor](#huffman-compressor)
  - [Table of Contents](#table-of-contents)
  - [Features](#features)
    - [Highlights \& Optimizations](#highlights--optimizations)
    - [Huffman Coding Algorithm](#huffman-coding-algorithm)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Building](#building)
  - [Usage Examples](#usage-examples)
  - [Project Structure](#project-structure)
  - [Design \& Implementation Details](#design--implementation-details)
    - [Compression Process](#compression-process)
    - [Decompression Process](#decompression-process)
    - [Dictionary Format](#dictionary-format)
    - [Data Integrity](#data-integrity)
  - [Development Tasks \& Roadmap](#development-tasks--roadmap)

---

## Features

The Huffman Compressor implements a complete compression and decompression pipeline with the following capabilities:

- Frequency analysis of input files to build optimal Huffman trees
- Min-heap data structure for efficient tree construction
- Binary tree-based encoding and decoding
- Dictionary serialization for storing tree structure in compressed files
- XOR checksum validation for data integrity
- Padding bit management for byte-aligned output
- Support for any file type (text, images, binary data)
- Buffered I/O for efficient processing of large files

### Highlights & Optimizations

- Uses a min-heap to efficiently construct the Huffman tree by always selecting the two nodes with lowest frequencies
- Dictionary is stored as a compact binary representation in the file header
- Buffered reading and writing (4-8KB buffers) minimize I/O operations
- XOR checksum detects file corruption during decompression
- Smart padding handling prevents false character decoding from padded zeros
- Memory-efficient tree reconstruction from dictionary string

### Huffman Coding Algorithm

Huffman coding achieves compression by:

1. Analyzing character frequencies in the input file
2. Building a binary tree where frequent characters have shorter codes
3. Generating variable-length binary codes (prefix-free property ensures no code is a prefix of another)
4. Encoding the original data using these codes
5. Storing the tree structure (dictionary) alongside the compressed data

The algorithm guarantees optimal prefix-free codes for the given character frequencies.

---

## Getting Started

### Prerequisites

- A **C compiler** supporting C99 (e.g. `gcc`, `clang`)
- `make` (GNU Make)
- Tools for linting/formatting: `cppcheck`, `clang-format` (optional)

### Building

In the root of the project (where the `Makefile` lives), run:

```bash
make all
```

This will:

- Compile `.c` files in `src/` to object files under `bin/`
- Link them into the executable `bin/comp.out`

If you want to clean build artifacts:

```bash
make clean
```

You can also run linting checks:

```bash
make lint
```

and format the source:

```bash
make format
```

To run the test suite:

```bash
make test
```

---

## Usage Examples

**Compressing a text file:**

```bash
./bin/comp.out 0 test/pan-tadeusz.txt compressed.huff
```

**Decompressing back to original:**

```bash
./bin/comp.out 1 compressed.huff restored.txt
```

**Compressing an image:**

```bash
./bin/comp.out 0 photo.png photo.huff
./bin/comp.out 1 photo.huff photo_restored.png
```

The program works with any file type and preserves binary data integrity through checksum validation.

---

## Project Structure

```txt
├── .gitignore
├── .clang-format
├── Makefile
├── README.md
├── bin/
│   ├── comp.out
│   └── *.o
├── src/
│   ├── include/
│   │   ├── decoder.h
│   │   ├── dictionary.h
│   │   ├── encoder.h
│   │   ├── heap.h
│   │   └── node.h
│   ├── decoder.c
│   ├── dictionary.c
│   ├── encoder.c
│   ├── heap.c
│   ├── main.c
│   └── node.c
└── test/
    ├── test.txt
    ├── test.png
    └── pan-tadeusz.txt
```

- `src/*.c` and `src/include/*.h` — Implementation and header files
- `bin/` — Build artifacts and final executable
- `test/` — Sample files for testing compression/decompression
- `Makefile` — Build configuration with targets for compilation, testing, and linting

## Design & Implementation Details

### Compression Process

The compression pipeline follows these steps:

1. **Frequency Analysis** (`main.c`) — Read the input file and count occurrences of each byte value (0-255)

2. **Heap Construction** (`heap.c`) — Create a min-heap and insert leaf nodes for each character that appears in the file, with their frequencies

3. **Huffman Tree Building** (`node.c`) — Repeatedly extract the two minimum-frequency nodes, create a parent node with their combined frequency, and insert back into the heap until one node remains (the root)

4. **Code Generation** (`node.c`) — Traverse the tree to assign binary codes: '0' for left branches, '1' for right branches. Leaf nodes receive their final codes

5. **Dictionary Generation** (`dictionary.c`) — Serialize the tree structure into a compact binary string representation for storage in the compressed file

6. **File Encoding** (`encoder.c`) — Read the input file, replace each byte with its Huffman code, pack the bits into bytes, write the dictionary header and compressed data, compute and store XOR checksum

### Decompression Process

The decompression pipeline reverses the process:

1. **Dictionary Parsing** (`dictionary.c`) — Read the file header to extract the dictionary string and metadata (padding bits, checksum)

2. **Tree Reconstruction** (`node.c`) — Parse the dictionary string to rebuild the Huffman tree structure

3. **Checksum Validation** (`dictionary.c`) — Compute XOR of all bytes and verify against stored checksum to detect corruption

4. **File Decoding** (`decoder.c`) — Read compressed data, convert bytes to binary string, traverse the tree using the bits until reaching a leaf node, write the decoded character, repeat until file is fully processed

5. **Padding Correction** (`decoder.c`) — Handle padding bits added during compression to ensure byte alignment, removing any false characters that may have been decoded from padding

### Dictionary Format

The compressed file begins with a header containing:

- **Byte 1**: XOR checksum value (computed from all subsequent bytes)
- **Byte 2**: First 3 bits encode the number of padding bits (0-7), remaining 5 bits are part of the tree encoding
- **Remaining header bytes**: Binary tree representation where '0' indicates an internal node (followed by left and right subtrees) and '1' indicates a leaf node (followed by 8 bits representing the character value)

This format allows efficient reconstruction of the tree structure during decompression.

### Data Integrity

- **XOR Checksum**: Every byte in the compressed file (except the first two) is XORed with a base value ('a'). The result is stored as the first byte, allowing detection of file corruption
- **Padding Validation**: The decoder checks if padded zeros could be misinterpreted as valid codes and truncates false characters if necessary
- **File Structure Validation**: The dictionary parser verifies that the number of leaf nodes equals the number of non-leaf nodes plus one (property of binary trees)

## Development Tasks & Roadmap

Potential enhancements to consider:

- Add support for streaming compression (process files larger than available memory)
- Add multi-threading for parallel compression of large files
- Implement additional compression algorithms (LZW, arithmetic coding) for comparison
- Add directory compression with recursive file handling
- Port to Windows with native API calls (currently uses POSIX `ftruncate`)
- Add comprehensive benchmark suite comparing compression ratios and speeds