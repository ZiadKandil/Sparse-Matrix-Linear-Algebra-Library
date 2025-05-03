#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <vector>
#include <map>

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

// Matrix class template
template<typename T, StorageOrder order>
class matrix{
    private:
    unsigned int rows;  // Number of rows
    unsigned int cols;  // Number of columns
    std::map<std::array<int, 2>, T, array_comparison<order>> data;  // Matrix indices and values sorted using comparison functor
    bool compressed = false;  // Indicate if the matrix is compressed

    // Compression Vectors
    std::vector<T> values;
    std::vector<unsigned int> inner_indices;
    std::vector<unsigned int> outer_start;

    public:
    //Constructor
    matrix(unsigned int r, unsigned int c):
     rows(r), cols(c) {}

    // Set the value of a given indices
    void insert(unsigned int i, unsigned int j, T value){

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
    unsigned int get_rows() const {
        return rows;
    }

    // Get number of columns
    unsigned int get_cols() const {
        return cols;
    }


    // Extracting a row from the matrix
    std::vector<T> extract_row(unsigned int r) const{
        std::vector<T> row(cols,T()); // Initialize the row with default values
        for (unsigned int i = 0; i < cols; ++i){
            auto it = data.find({r,i});
            if (it != data.end()){
                row[i] = it->second; // If the value exists, assign it to the row
            }
        }
        return row;
    }

      // Extracting a column from the matrix
      std::vector<T> extract_column(unsigned int c) const{
        std::vector<T> column(rows,T()); // Initialize the column with default values
        for (unsigned int i = 0; i < rows; ++i){
            auto it = data.find({i,c});
            if (it != data.end()){
                column[i] = it->second; // If the value exists, assign it to the row
            }
        }
        return column;
    }

    // Compressing the matrix
    void compress(){
        // Check if the matrix is already compressed
        if (compressed){
            std::cout<< "Matrix already compressed" << std::endl;
            return;
        }

        // Initialize the compression vectors
        unsigned int nnz = data.size();
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

            for (unsigned int i = 1; i <= rows; ++i){
                outer_start[i] += outer_start[i - 1];   // Cumulative sum to get the start index of each row
            }

            std::vector<unsigned int> current_idx = outer_start;  // Initialize current index for each row

            for (auto& [key, value] : data){
                unsigned int row = key[0];
                unsigned int idx = current_idx[row]; // Get the current index for the row
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

            std::vector<unsigned int> current_idx = outer_start;  // Initialize current index for each column

            for (auto& [key, value] : data){
                unsigned int col = key[1];
                unsigned int idx = current_idx[col]; // Get the current index for the column
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

            for (unsigned int row = 0; row < rows; ++row){
                unsigned int row_start = outer_start[row];  //first index of the row in values vector
                unsigned int row_end = outer_start[row + 1];  // last index of the row in values vector

                for (unsigned int idx = row_start; idx < row_end; ++idx){
                    unsigned int col = inner_indices[idx];  // Column index
                    T value = values[idx];  // values in the row
                    data[{row, col}] = value;  // Insert the value into the data map
                }
            }
        }
        else{
            for (unsigned int col = 0; col < cols; ++col){
                unsigned int col_start = outer_start[col];  //first index of the col in values vector
                unsigned int col_end = outer_start[col + 1];  // last index of the col in values vector

                for (unsigned int idx = col_start; idx < col_end; ++idx){
                    unsigned int row = inner_indices[idx];  // row index
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
    T operator() (unsigned int i, unsigned int j) const{
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
            unsigned int row_start = outer_start[i];  // first index of the row in values vector
            unsigned int row_end = outer_start[i + 1];  // last index of the row in values vector
            for (unsigned int idx = row_start; idx < row_end; ++idx){
                // Check if the column index matches
                if (inner_indices[idx] == j){
                    return values[idx];  // Return the value at the given indices
                }
            }
        }
        else{
            unsigned int col_start = outer_start[j];  // first index of the col in values vector
            unsigned int col_end = outer_start[j + 1];  // last index of the col in values vector
            for (unsigned int idx = col_start; idx < col_end; ++idx){
                // Check if the row index matches
                if (inner_indices[idx] == i){
                    return values[idx];  // Return the value at the given indices
                }
            }
        }
        return T();  // Return default value if not found
    }

    // call operator non-const version
    T& operator() (unsigned int i, unsigned int j){
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
    void resize(unsigned int new_rows, unsigned int new_cols){
        if (compressed){
            this->uncompress(); // Uncompress the matrix before resizing
        }
        for (auto it = data.begin(); it != data.end();){
            unsigned int i = it->first[0];
            unsigned int j = it->first[1];
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

    // Print the matrix
    void print() const{
        if (compressed){
            throw std::runtime_error("Matrix is compressed, Please uncompress first"); // Uncompress the matrix before printing
        }

        std::cout<< "[ "<<std::endl;

        for (unsigned int i = 0; i < rows; ++i){
            for (unsigned int j = 0; j < cols; ++j){
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

template<typename T, StorageOrder order>
std::vector<T> operator*(const matrix<T, order>& Mat, const std::vector<T>& vec){
    // Check if the dimensions of the matrix and vector match
    if (Mat.get_cols() != vec.size()){
        throw std::invalid_argument("Matrix and vector dimensions do not match");
    }

    std::vector<T> result(Mat.get_rows(), T()); // Initialize the result vector with default values

    // Check if the matrix is not compressed
    if (!Mat.is_compressed()){
        Mat.compress(); // Compress the matrix before multiplication
    }

    if constexpr (order == StorageOrder::row_major){
        // Row-major order multiplication (Classic)
        for (unsigned int i = 0; i < Mat.get_rows(); ++i){
            unsigned int row_start = Mat.outer_start[i];  // first index of the current row in values vector
            unsigned int row_end = Mat.outer_start[i + 1];  // last index of the current row in values vector
            // loop over the non-zero values in the current row
            for (unsigned int idx = row_start; idx < row_end; ++idx){
                unsigned int col = Mat.inner_indices[idx];  // Column index
                T value = Mat.values[idx];  // Value in the current row
                result[i] += value * vec[col];  // Multiply and accumulate the result
            }
        }
    }
    else{
        // Column-major order multiplication
        for (unsigned int j = 0; j < Mat.get_cols(); ++j){
            unsigned int col_start = Mat.outer_start[j];  // first index of the current column in values vector
            unsigned int col_end = Mat.outer_start[j + 1];  // last index of the current column in values vector
             // loop over the non-zero values in the current column
             for (unsigned int idx = col_start; idx < col_end; ++idx){
                unsigned int row = Mat.inner_indices[idx];  // row index
                T value = Mat.values[idx];  // Value in the current column
                result[row] += value * vec[j];  // Multiply and accumulate the result
            }
        }
    }
    return result;
}

};

#endif