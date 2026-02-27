# Sparse Matrix Linear Algebra Library

A high-performance C++ template library designed for sparse matrix operations, supporting various storage formats and zero-copy views. The code is thoroughly commented for clarity. Test cases are included in the main file to assess performance across different scenarios. Future extensions could include operations like matrix-matrix multiplication.


## Table of Contents
1. [Features](#features)
2. [Requirements](#requirements)
3. [Installation](#installation)
4. [Usage](#usage)
5. [Performance](#performance)
6. [License](#license)

## Features

#### Core Functionality
- **Matrix Storage Formats**
  - Coordinate format (COO)
  - Compressed Sparse Row (CSR)
  - Compressed Sparse Column (CSC)

- **Matrix Operations**
  - Element insertion/access
  - Matrix-vector multiplication
  - Matrix-column matrix multiplication
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

## Usage

### Basic Matrix Creation and Element Access

```cpp
#include "matrix.hpp"
using namespace algebra;

// Create a 10x10 row-major sparse matrix
matrix<double, StorageOrder::row_major> mat(10, 10);

// Insert elements using insert() method
mat.insert(0, 0, 5.0);
mat.insert(1, 1, 3.5);
mat.insert(2, 5, 7.2);

// Or use the () operator
mat(3, 3) = 4.0;  // Auto-resizes if needed

// Access elements (const version)
double value = mat(0, 0);  // Returns 5.0
double zero = mat(5, 5);   // Returns 0.0 (default for non-existent elements)
```

### Matrix Compression

Compress the matrix to CSR/CSC format for better performance:

```cpp
// Compress the matrix
mat.compress();

// Check compression status
bool is_compressed = mat.is_compressed();

// Uncompress when needed
mat.uncompress();

// Note: insert() and non-const operator() automatically uncompress
```

### Matrix-Vector Multiplication

```cpp
// Create a vector
std::vector<double> vec(10, 1.0);

// Multiply (works on both compressed and uncompressed matrices)
auto result = mat * vec;

// For better performance, compress first
mat.compress();
auto result_fast = mat * vec;
```

### Computing Norms

```cpp
// Frobenius norm
double frob_norm = mat.norm<NormType::Frobenius>();

// 1-norm (maximum absolute column sum)
double one_norm = mat.norm<NormType::One>();

// Infinity norm (maximum absolute row sum)
double inf_norm = mat.norm<NormType::Infinity>();
```

### Reading from Matrix Market Files

```cpp
matrix<double, StorageOrder::row_major> mat;
mat.read("./Data/matrix.mtx");

std::cout << "Rows: " << mat.get_rows() << std::endl;
std::cout << "Cols: " << mat.get_cols() << std::endl;
std::cout << "Non-zeros: " << mat.get_nnz() << std::endl;
```

### Using Views

#### Transpose View

```cpp
#include "transpose_view.hpp"

matrix<double, StorageOrder::row_major> mat(3, 3);
mat(0, 0) = 1; mat(0, 1) = 2; mat(0, 2) = 3;
mat(1, 0) = 4; mat(1, 1) = 5; mat(1, 2) = 6;
mat(2, 0) = 7; mat(2, 1) = 8; mat(2, 2) = 9;

// Create transpose view (zero-copy)
transpose_view<double, StorageOrder::row_major> mat_T(mat);

// Access transposed elements
double val = mat_T(1, 0);  // Returns mat(0, 1) = 2

// Print transpose
mat_T.print();
```

#### Diagonal View

```cpp
#include "diagonal_view.hpp"

// Create diagonal view
diagonal_view<double, StorageOrder::row_major> diag(mat);

// Access diagonal elements
double d0 = diag(0);  // Returns mat(0, 0)
double d1 = diag(1);  // Returns mat(1, 1)

// Print diagonal
diag.print();
```

### Column-Major Matrices

```cpp
// Create column-major matrix (uses CSC when compressed)
matrix<double, StorageOrder::column_major> col_mat(10, 10);

// Same operations as row-major
col_mat.insert(0, 0, 5.0);
col_mat.compress();

// Column-major is more efficient for column-wise operations
```

### Working with Complex Numbers

```cpp
#include <complex>

// Complex matrix
matrix<std::complex<double>, StorageOrder::row_major> cmat(5, 5);

cmat.insert(0, 0, std::complex<double>(1.0, 2.0));
cmat.insert(1, 1, std::complex<double>(3.0, 4.0));

// All operations work with complex numbers
cmat.compress();
double norm = cmat.norm<NormType::Frobenius>();
```

### Performance Tips

1. **Compress after bulk insertions**: Build the matrix with `insert()`, then call `compress()` before performing operations
2. **Use appropriate storage order**: Row-major for row-wise operations, column-major for column-wise
3. **Const access for reading**: Use const references when reading from compressed matrices to avoid automatic decompression
4. **Batch insertions**: Insert all elements before compressing, rather than alternating between insertion and compression

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

CC0 1.0 Universal (Creative Commons) - See included LICENSE file for details.