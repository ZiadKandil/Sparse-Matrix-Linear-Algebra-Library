#ifndef TRANSPOSE_VIEW_HPP
#define TRANSPOSE_VIEW_HPP

#include "matrix.hpp"

namespace algebra {

template<typename T, StorageOrder order>
class transpose_view{
    private:
        const matrix<T, order>& mat;

    public:
        // Constructor
        transpose_view(const matrix<T, order>& m): mat(m) {}

        // Number of rows
        std::size_t get_rows() const{
            return mat.get_cols();
        }

        // Number of columns
        std::size_t get_cols() const{
            return mat.get_rows();
        }

        // Function to get the transposed element
        T operator()(std::size_t i, std::size_t j) const{
            // Check if the indices are within bounds
            if (i >= mat.get_cols() || j >= mat.get_rows()){
                throw std::out_of_range("Index out of bounds");
            }
            return mat(j,i);
        }

        // print the transposed matrix
        void print() const{
            std::cout<<"[ "<<std::endl;
            for (std::size_t i = 0; i < this->get_rows(); ++i){
                for (std::size_t j = 0; j < this->get_cols(); ++j){
                    std::cout<< (*this)(i,j)<<" ";
                }
                std::cout<<std::endl;
            }
            std::cout<<"]"<<std::endl;
        }
};
}

#endif