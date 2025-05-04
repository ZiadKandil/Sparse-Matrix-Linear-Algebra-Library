#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <vector>
#include <map>
#include <type_traits>
#include <complex>
#include <fstream>
#include <sstream>
#include <string>

//namespace algebra
namespace algebra {

// Enumerator that indicates the storage order of the matrix
enum class StorageOrder{row_major, column_major};

//Defining a functor as comparison operator for std::array<int,2>
template< StorageOrder order>
struct array_comparison{
    bool operator()(const std::array<int,2> &a, const std::array<int,2> &b) const {
        if constexpr (order == StorageOrder::row_major){
            return (a[0] < b[0]) || (a[0] == b[0] && a[1] < b[1]);
        }
        else{
            return (a[1] < b[1]) || (a[1] == b[1] && a[0] < b[0]);
        }
    }
};

// Trait to check if a type is arithmetic or complex arithmetic
template<typename T>
struct is_arithmetic_or_complex: std::is_arithmetic<T> {};
// Specialization for std::complex
template<typename T>
struct is_arithmetic_or_complex<std::complex<T>>: std::is_arithmetic<T> {};

// Matrix class template
template<typename T, StorageOrder order>
class matrix{
    private:
    std::size_t rows;  // Number of rows
    std::size_t cols;  // Number of columns
    std::map<std::array<int, 2>, T, array_comparison<order>> data;  // Matrix indices and values sorted using comparison functor
    bool compressed = false;  // Indicate if the matrix is compressed

    // Compression Vectors
    std::vector<T> values;
    std::vector<std::size_t> inner_indices;
    std::vector<std::size_t> outer_start;

    public:

    // Limit to arithmetic or complex types
    static_assert(is_arithmetic_or_complex<T>::value, "Matrix can only be of arithmetic or complex types");

    //Constructor
    matrix(std::size_t r, std::size_t c):
     rows(r), cols(c) {}

     // Default copy constructor
    matrix(const matrix& rhs) = default;

    // Default move constructor
    matrix(matrix&& rhs) = default;

    // Default copy assignment operator
    matrix& operator=(const matrix& rhs) = default;

    // Default move assignment operator
    matrix& operator=(matrix&& rhs) = default;

    // Set the value of a given indices
    void insert(std::size_t i, std::size_t j, T value){

        // Keeping the matrix Sparse
        if (value == T()){
            return; // Do not insert zero values
        }
        // check if the matrix is compressed
        if (compressed){
            this->uncompress();  // Uncompress the matrix before inserting
        }
        if (i >= rows){
            rows = i + 1;
        }
        if (j >= cols){
            cols = j + 1;
        }
        data[{i,j}] = value;
    }

    // Get number of rows
    std::size_t get_rows() const {
        return rows;
    }

    // Get number of columns
    std::size_t get_cols() const {
        return cols;
    }


    // Extracting a row from the matrix
    std::vector<T> extract_row(std::size_t r) const{
        std::vector<T> row(cols,T()); // Initialize the row with default values

        // Matrix not compressed
        if (!compressed){
            for (std::size_t i = 0; i < cols; ++i){
                auto it = data.find({r,i});
                if (it != data.end()){
                    row[i] = it->second; // If the value exists, assign it to the row
                }
            }
            return row;
        }
        else{
            // Matrix compressed
            if constexpr (order == StorageOrder::row_major){
                std::size_t row_start = outer_start[r];  // first index of the row in values vector
                std::size_t row_end = outer_start[r+1];  // last index of the row in values vector
                for (std::size_t idx = row_start; idx < row_end; ++idx){
                    std::size_t col = inner_indices[idx];  // column index
                    T value = values[idx];  // value in the row
                    row[col] = value;  // Assign the value to the row in the right place
                }
                return row;
            }
            else
            {
                // loop over all columns
                for (std::size_t c = 0; c < cols; ++c){
                    std::size_t col_start = outer_start[c];  // first index of the column in values vector
                    std::size_t col_end = outer_start[c + 1];  // last index of the column in values vector
                    // loop over all non-zero rows in the current column
                    for (std::size_t idx = col_start; idx < col_end; ++idx){
                        std::size_t row_idx = inner_indices[idx];  // row index
                        if (row_idx == r){
                            T value = values[idx];  //value in the column
                            row[c] = value;  // Assign the value to the row in the right place
                        }
                    }
                }
                return row;
            }
        }
    }


      // Extracting a column from the matrix
      std::vector<T> extract_column(std::size_t c) const{
        std::vector<T> column(rows,T()); // Initialize the column with default values
        // Matrix not compressed
        if (!compressed){
            for (std::size_t i = 0; i < rows; ++i){
                auto it = data.find({i,c});
                if (it != data.end()){
                    column[i] = it->second; // If the value exists, assign it to the row
                }
            }
            return column;
        }
        else{
            if constexpr (order == StorageOrder::row_major){
                // loop over all rows
                for (std::size_t r = 0; r < rows; ++r){
                    std::size_t row_start = outer_start[r];  // first index of the row in values vector
                    std::size_t row_end = outer_start[r+1];  // last index of the row in values vector
                    // loop over all non-zero columns in the current row
                    for (std::size_t idx = row_start; idx < row_end; ++idx){
                        std::size_t col_idx = inner_indices[idx];  // column index
                        if (col_idx == c){
                            T value = values[idx];  // value in the row
                            column[r] = value;  // Assign the value to the column in the right place
                        }
                    }
                }
                return column;
            }
            else{
                std::size_t col_start = outer_start[c];  // first index of the column in values vector
                std::size_t col_end = outer_start[c+1];  // last index of the column in values vector
                for (std::size_t idx = col_start; idx < col_end; ++idx){
                    std::size_t row = inner_indices[idx];  // row index
                    T value = values[idx];  // value in the column
                    column[row] = value;  // Assign the value to the column in the right place
                }
                return column;
            }
        }
    }

    // Compressing the matrix
    void compress(){
        // Check if the matrix is already compressed
        if (compressed){
            std::cout<< "Matrix already compressed" << std::endl;
            return;
        }

        // Initialize the compression vectors
        std::size_t nnz = data.size();
        values.clear();
        inner_indices.clear();
        outer_start.clear();

        values.resize(nnz);
        inner_indices.resize(nnz);
        // Initialize outer_start with zeros according to storage order
        outer_start.resize((order == StorageOrder::row_major ? rows : cols) + 1, 0);  


        if constexpr (order == StorageOrder::row_major){
            
            for (auto& [key, value] : data){
                outer_start[key[0] + 1]++;        // count all entries for each row
            }

            for (std::size_t i = 1; i <= rows; ++i){
                outer_start[i] += outer_start[i - 1];   // Cumulative sum to get the start index of each row
            }

            std::vector<std::size_t> current_idx = outer_start;  // Initialize current index for each row

            for (auto& [key, value] : data){
                std::size_t row = key[0];
                std::size_t idx = current_idx[row]; // Get the current index for the row
                values[idx] = value; // Assign the value to the compressed vector
                inner_indices[idx] = key[1];  // Assign the column index
                current_idx[row]++; // Increment the current index for the row
            }
        }
        else{
            
            for (auto& [key, value] : data){
                outer_start[key[1] + 1]++;        // count all entries for each column
            }

            for (unsigned int i = 1; i <= cols; ++i){
                outer_start[i] += outer_start[i - 1];   // Cumulative sum to get the start index of each column
            }

            std::vector<std::size_t> current_idx = outer_start;  // Initialize current index for each column

            for (auto& [key, value] : data){
                std::size_t col = key[1];
                std::size_t idx = current_idx[col]; // Get the current index for the column
                values[idx] = value; // Assign the value to the compressed vector
                inner_indices[idx] = key[0];  // Assign the row index
                current_idx[col]++; // Increment the current index for the column
            }         

        }

        data.clear(); // Clear the original data map
        compressed = true; // Set the compressed bool to true

    }

    // uncompressing the matrix
    void uncompress(){

        // Check if the matrix is already uncompressed
        if (!compressed){
            std::cout<< "Matrix already uncompressed" << std::endl;
            return;
        }

        data.clear();  // Clear the original data map

        if constexpr (order == StorageOrder::row_major){

            for (std::size_t row = 0; row < rows; ++row){
                std::size_t row_start = outer_start[row];  //first index of the row in values vector
                std::size_t row_end = outer_start[row + 1];  // last index of the row in values vector

                for (std::size_t idx = row_start; idx < row_end; ++idx){
                    std::size_t col = inner_indices[idx];  // Column index
                    T value = values[idx];  // values in the row
                    data[{row, col}] = value;  // Insert the value into the data map
                }
            }
        }
        else{
            for (std::size_t col = 0; col < cols; ++col){
                std::size_t col_start = outer_start[col];  //first index of the col in values vector
                std::size_t col_end = outer_start[col + 1];  // last index of the col in values vector

                for (std::size_t idx = col_start; idx < col_end; ++idx){
                    std::size_t row = inner_indices[idx];  // row index
                    T value = values[idx];  // values in the column
                    data[{row, col}] = value;  // Insert the value into the data map
                }
            }
        }

        values.clear();  // Clear the compressed values vector
        inner_indices.clear();  // Clear the compressed inner indices vector
        outer_start.clear();  // Clear the compressed outer start vector
        compressed = false;  // Set the compressed bool to false

    }

    // Compression status
    bool is_compressed() const{
        return compressed; 
    }

    // call operator const version
    T operator() (std::size_t i, std::size_t j) const{
        // Check indices
        if (i >= rows || j >= cols){
            throw std::out_of_range("Index out of range");
        }

        // if the matrix is uncompressed
        if (!compressed){
            
            auto it = data.find({i,j});
            if (it != data.end()){
                return it->second;  // Return the value at the given indices
            }
            else
            {
                return T();  // Return default value
            }
        }

        // If the matrix is compressed
        if constexpr (order == StorageOrder::row_major){
            std::size_t row_start = outer_start[i];  // first index of the row in values vector
            std::size_t row_end = outer_start[i + 1];  // last index of the row in values vector
            for (std::size_t idx = row_start; idx < row_end; ++idx){
                // Check if the column index matches
                if (inner_indices[idx] == j){
                    return values[idx];  // Return the value at the given indices
                }
            }
        }
        else{
            std::size_t col_start = outer_start[j];  // first index of the col in values vector
            std::size_t col_end = outer_start[j + 1];  // last index of the col in values vector
            for (std::size_t idx = col_start; idx < col_end; ++idx){
                // Check if the row index matches
                if (inner_indices[idx] == i){
                    return values[idx];  // Return the value at the given indices
                }
            }
        }
        return T();  // Return default value if not found
    }

    // call operator non-const version
    T& operator() (std::size_t i, std::size_t j){
        if (compressed){
            this->uncompress(); // Uncompress the matrix before accessing
        }
        if (i >= rows){
            rows = i + 1;
        }
        if (j >= cols){
            cols = j + 1;
        }
        return data[{i,j}];  // Return the value at the given indices
    }

    // resize the matrix
    void resize(std::size_t new_rows, std::size_t new_cols){
        if (compressed){
            this->uncompress(); // Uncompress the matrix before resizing
        }
        for (auto it = data.begin(); it != data.end();){
            std::size_t i = it->first[0];
            std::size_t j = it->first[1];
            if (i >= new_rows || j >= new_cols){
                it = data.erase(it);  // Remove the element if it is out of bounds
            }
            else{
                ++it;
            }
        }
        rows = new_rows;
        cols = new_cols;
    }

    // Declaration friend operator for multiplication of matrix with a std::vector
    friend std::vector<T> operator*(const matrix<T, order>& Mat, const std::vector<T>& vec);
    

    // method to read matrix from matrix market
    void read (const std::string& file_name){

        // put the file in a ifstream object
        std::ifstream file (file_name);
        // check if the file is open
        if (!file.is_open()){
            throw std::runtime_error("file is not open");
        }

        // Read the header
        std::string line;
        while (std::getline(file, line)){
            // skip comments and empty lines
            if (line.empty() || line[0] == '%'){
                continue;
            }
            break;  // break the loop if we reach the first line of the matrix 
        }

        std::size_t r;
        std::size_t c;
        std::size_t nnz;
        std::istringstream iss(line);

        // assign matrix dimensions
        iss >> r >> c >> nnz;

        // Resize the matrix for efficient inserting
        this->resize(r, c);

        // Read the non zero values and the corresponding indices from the file
        std::size_t i;
        std::size_t j;
        T value;
        while (file >> i >> j >> value){
            this->insert(i-1, j-1, value);  // Insert the values into the matrix (1-based to 0-based index)
        }

        file.close();
    }

    // Print the matrix
    void print() const{
        if (compressed){
            throw std::runtime_error("Matrix is compressed, Please uncompress first"); // Uncompress the matrix before printing
        }

        std::cout<< "[ "<<std::endl;

        for (std::size_t i = 0; i < rows; ++i){
            for (std::size_t j = 0; j < cols; ++j){
                auto it = data.find({i,j});
                if (it != data.end()){
                    std::cout<< it->second<<" ";
                }
                else{
                    std::cout<< T();  // Print default value for missing values
                }
            }
            std::cout<< std::endl;
        }
        std::cout<< " ]" << std::endl;
    }
};

template<typename T1, StorageOrder order, typename T2>
requires is_arithmetic_or_complex<T1>::value && is_arithmetic_or_complex<T2>::value  // limitation to arithmetic or complex types
auto operator*(const matrix<T1, order>& Mat, const std::vector<T2>& vec){

    // Matrix and vector types must be compatible
    static_assert(std::is_convertible_v<T1, T2> || std::is_convertible_v<T2, T1>, "Matrix and vector types must be compatible");
    // Check if the dimensions of the matrix and vector match
    if (Mat.get_cols() != vec.size()){
        throw std::invalid_argument("Matrix and vector dimensions do not match");
    }

    using result_type = std::common_type_t<T1, T2>;

    std::vector<result_type> result(Mat.get_rows(), result_type{}); // Initialize the result vector with default values

    //  matrix is not compressed
    if (!Mat.is_compressed()){
        // Loop over the non-zero values in the matrix
        for (const auto& [key, value]: Mat.data){
            std::size_t row = key[0];
            std::size_t col = key[1];
            result[row] += value * vec[col];  
        }
        return result;
       
    }

    // matrix is compressed
    if constexpr (order == StorageOrder::row_major){
        // Row-major order multiplication (Classic)
        for (std::size_t i = 0; i < Mat.get_rows(); ++i){
            std::size_t row_start = Mat.outer_start[i];  // first index of the current row in values vector
            std::size_t row_end = Mat.outer_start[i + 1];  // last index of the current row in values vector
            // loop over the non-zero values in the current row
            for (std::size_t idx = row_start; idx < row_end; ++idx){
                std::size_t col = Mat.inner_indices[idx];  // Column index
                result_type value = Mat.values[idx];  // Value in the current row
                result[i] += value * vec[col];  // Multiply and accumulate the result
            }
        }
        return result;
    }
    else{
        // Column-major order multiplication
        for (std::size_t j = 0; j < Mat.get_cols(); ++j){
            std::size_t col_start = Mat.outer_start[j];  // first index of the current column in values vector
            std::size_t col_end = Mat.outer_start[j + 1];  // last index of the current column in values vector
             // loop over the non-zero values in the current column
             for (std::size_t idx = col_start; idx < col_end; ++idx){
                std::size_t row = Mat.inner_indices[idx];  // row index
                result_type value = Mat.values[idx];  // Value in the current column
                result[row] += value * vec[j];  // Multiply and accumulate the result
            }
        }
        return result;
    }
}

};

#endif