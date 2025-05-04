#ifndef DIAGONAL_VIEW_HPP
#define DIAGONAL_VIEW_HPP

#include "matrix.hpp"

namespace algebra {

template<typename T, StorageOrder order>
class diagonal_view{

    private:
       const matrix<T, order>& mat;

    public:
        // constructor
        diagonal_view(const matrix<T, order>& m): mat(m) {}

        // Function to get the diagonal elements
        T operator()(std::size_t i) const{
            // Check if the index is within bounds
            if (i >= mat.get_rows() || i >= mat.get_cols()){
                throw std::out_of_range("Index out of bounds");
            }
            return mat(i,i);  // Return the diagonal element
        }

        // Function to get the size of the diagonal
        std::size_t size() const{
            return std::min(mat.get_rows(), mat.get_cols());  // Return the minimum of rows and columns
        }

        // Print the diagonal elements
        void print() const{
            std::cout<<"[ ";
            for (std::size_t i = 0; i < size(); ++i){
                std::cout<<mat(i,i)<<" ";
            }
            std::cout<<"]"<<std::endl;
        }

};
}

#endif