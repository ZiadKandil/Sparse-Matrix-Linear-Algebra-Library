# Sparse Matrix Linear Algebra Library

## Table of Contents
1. [Features](#features)
2. [Requirements](#requirements)
3. [Installation](#installation)
4. [Basic Usage](#basic-usage)
5. [Advanced Features](#advanced-features)
6. [Performance](#performance)
7. [License](#license)

## Features

### Core Functionality
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

## Basic Usage

### Creating Matrices
- Specify dimensions and storage order
- Insert non-zero elements
- Optional compression

### Accessing Elements
- Direct element access
- Row/column extraction
- View operations

### File Operations
- Read matrices from .mtx files
- Automatic dimension detection

## Advanced Features

### Performance Optimization
- Compression for large matrices
- Batch operations
- Memory-efficient views

### Custom Operations
- Norm type selection
- Storage order optimization
- Custom comparison functions

## Performance

### Typical Results
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