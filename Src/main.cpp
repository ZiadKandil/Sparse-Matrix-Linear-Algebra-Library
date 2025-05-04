#include "matrix.hpp"

using namespace algebra;

int main(){

    matrix<double, StorageOrder::row_major> mat1(4,4);
    matrix<double, StorageOrder::column_major> mat2(3,3);
    
    mat1(1,1) = 7;  // testing non-const call operator on row major matrix
    mat1.insert(2,2,7); // testing insert on row major matrix
    mat1.compress();
    mat1.insert(0,0,7);  // testing insert on row major matrix after compression
    mat1(3,3) = 7;  // testing non-const call operator on row major matrix after compression

    mat2(1,1) = 8;  // testing non-const call operator on column major matrix
    mat2.insert(2,2,8); // testing insert on column major matrix
    mat2.compress();
    mat2.insert(0,0,8);  // testing insert on column major matrix after compression
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
    


    return 0;
}
