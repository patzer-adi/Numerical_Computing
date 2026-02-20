#include "../include/Matrix.hpp"


// operator+ — lets us write  c = a + b;
// Throws MatrixError if sizes don't match
Matrix Matrix::operator+(const Matrix& other) {

    if (rows != other.rows || cols != other.cols) {
        throw MatrixError("Addition failed! Matrices must be the same size.");
    }

    Matrix result(rows, cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result.data[i][j] = data[i][j] + other.data[i][j];
        }
    }

    return result;
}


// operator- — lets us write  c = a - b;
// Throws MatrixError if sizes don't match
Matrix Matrix::operator-(const Matrix& other) {

    if (rows != other.rows || cols != other.cols) {
        throw MatrixError("Subtraction failed! Matrices must be the same size.");
    }

    Matrix result(rows, cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result.data[i][j] = data[i][j] - other.data[i][j];
        }
    }

    return result;
}