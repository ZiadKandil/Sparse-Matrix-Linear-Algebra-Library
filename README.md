# Sparse Matrix Linear Algebra Library

A high-performance C++ template library designed for sparse matrix operations, supporting various storage formats and zero-copy views. The code is thoroughly commented for clarity. Test cases are included in the main file to assess performance across different scenarios. Future extensions could include operations like matrix-matrix multiplication.


## Table of Contents
1. [Features](#features)
2. [Requirements](#requirements)
3. [Installation](#installation)
4. [Performance](#performance)
5. [License](#license)

## Features

#### Core Functionality
- **Matrix Storage Formats**
  - Coordinate format (COO)
  - Compressed Sparse Row (CSR)
  - Compressed Sparse Column (CSC)

- **Matrix Operations**
  - Element insertion/access
  - Matrix-vector multiplication
  - Norm calculations (1-norm, ∞-norm, Frobenius)
  - Compression/uncompression

- **View Operations**
  - Transpose view
  - Diagonal view

- **File I/O**
  - Matrix Market format support

## Requirements

- C++17 compatible compiler
- Standard Library components:
  - Containers (vector, map, array)
  - Complex numbers support
  - File streams
  - Chrono for benchmarking

## Installation

1. Download the header files
2. Add include path to your project
3. Include relevant headers as needed

## Performance

#### Typical Results
- **Compressed Storage**
  - Faster matrix operations
  - Reduced memory footprint

- **View Operations**
  - Zero-copy overhead
  - Constant time access

- **File I/O**
  - Efficient parsing
  - Automatic format detection

## License

MIT License - See included LICENSE file for details.