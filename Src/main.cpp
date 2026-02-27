#include "matrix.hpp"
#include "diagonal_view.hpp"
#include "transpose_view.hpp"
#include <chrono>

using namespace algebra;

int main(){

    matrix<double, StorageOrder::row_major> mat1(4,4);
    matrix<double, StorageOrder::column_major> mat2(3,3);
    
    mat1(1,1) = 7;  // testing non-const call operator on row major matrix
    mat1.insert(2,2,7); // testing insert on row major matrix
    mat1.compress();
    mat1.insert(0,0,7);  // testing insert on row major matrix after compression
    mat1.uncompress();
    mat1.insert(0,0,7);
    mat1(3,3) = 7;  // testing non-const call operator on row major matrix after compression

    mat2(1,1) = 8;  // testing non-const call operator on column major matrix
    mat2.insert(2,2,8); // testing insert on column major matrix
    mat2.compress();
    mat2.insert(0,0,8);  // testing insert on column major matrix after compression
    mat2.uncompress();
    mat2.insert(0,0,8);
    mat2(3,3) = 8;  // testing non-const call operator on column major matrix after compression

    //Printing matrices data
    std::cout << "Matrix 1 (Row Major):" << std::endl;
    mat1.print();
    std::cout << "Matrix 2 (Column Major):" << std::endl;
    mat2.print();

    // testing norm method
    std::cout << "Norm of mat1 (Frobenius): " << mat1.norm<NormType::Frobenius>() << std::endl;
    std::cout << "Norm of mat1 (One): " << mat1.norm<NormType::One>() << std::endl;
    std::cout << "Norm of mat1 (Infinity): " << mat1.norm<NormType::Infinity>() << std::endl;
    std::cout << "Norm of mat2 (Frobenius): " << mat2.norm<NormType::Frobenius>() << std::endl;
    std::cout << "Norm of mat2 (One): " << mat2.norm<NormType::One>() << std::endl;
    std::cout << "Norm of mat2 (Infinity): " << mat2.norm<NormType::Infinity>() << std::endl;

    // Read Matrix file from data folder
    matrix<double, StorageOrder::row_major> mat3;
    mat3.read("./Data/lnsp_131.mtx");

    // Print matrix dimensions
    std::cout << "Matrix 3 dimensions: " << mat3.get_rows() << " x " << mat3.get_cols() << std::endl;
    std::cout<<"Number of non-zero elements in matrix 3: " << mat3.get_nnz() << std::endl;

    // Creating a vector of ones of a suitable size for multiplication
    std::vector<double> vec(mat3.get_cols(), 1.0);

    // Measure time for multiplication for uncompressed matrix
    auto start1 = std::chrono::high_resolution_clock::now();
    auto result1 = mat3 * vec;  // Multiplication
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);

    // compress the matrix
    mat3.compress();

    // Measure time for multiplication for compressed matrix
    auto start2 = std::chrono::high_resolution_clock::now();
    auto result2 = mat3 * vec;  // Multiplication
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);

    std::cout << "Time taken for multiplication with uncompressed row matrix: " << duration1.count() << " µs" << std::endl;
    std::cout << "Time taken for multiplication with compressed row matrix: " << duration2.count() << " µs" << std::endl;

    // Repeating the test for column major matrix
    matrix<double, StorageOrder::column_major> mat4;
    mat4.read("./Data/lnsp_131.mtx");

    // Print matrix dimensions
    std::cout << "Matrix 4 dimensions: " << mat4.get_rows() << " x " << mat4.get_cols() << std::endl;
    std::cout<<"Number of non-zero elements in matrix 4: " << mat4.get_nnz() << std::endl;

    auto start3 = std::chrono::high_resolution_clock::now();
    auto result3 = mat4 * vec; 
    auto end3 = std::chrono::high_resolution_clock::now();
    auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(end3 - start3);

    mat4.compress();

    auto start4 = std::chrono::high_resolution_clock::now();
    auto result4 = mat4 * vec; 
    auto end4 = std::chrono::high_resolution_clock::now();
    auto duration4 = std::chrono::duration_cast<std::chrono::microseconds>(end4 - start4);

    std::cout << "Time taken for multiplication with uncompressed column matrix: " << duration3.count() << " µs" << std::endl;
    std::cout << "Time taken for multiplication with compressed column matrix: " << duration4.count() << " µs" << std::endl;

    // Testing transpose_view
    matrix<double, StorageOrder::row_major> mat5(3, 3);
    mat5(0, 0) = 1;
    mat5(0, 1) = 2;
    mat5(0, 2) = 3;
    mat5(1, 0) = 4;
    mat5(1, 1) = 5;
    mat5(1, 2) = 6;
    mat5(2, 0) = 7;
    mat5(2, 1) = 8;
    mat5(2, 2) = 9;
    transpose_view<double, StorageOrder::row_major> mat5_T(mat5);
    std::cout<< "Matrix 5 (Row Major):" << std::endl;
    mat5.print();
    std::cout<< "Transposed Matrix 5:" << std::endl;
    mat5_T.print();

    // Testing diagonal_view
    diagonal_view<double, StorageOrder::row_major> mat5_diag(mat5);
    std::cout<< "Diagonal of Matrix 5:" << std::endl;
    mat5_diag.print();
    
    return 0;
}
