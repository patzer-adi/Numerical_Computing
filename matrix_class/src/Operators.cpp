#include "../include/Matrix.hpp"

Matrix Matrix::operator+(const Matrix& other) {

    if (rows != other.rows || cols != other.cols) {
        throw MatrixError("Haiyaa!! Addition Failure dimensions should be same");
    }

    Matrix result(rows, cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result.data[i][j] = data[i][j] + other.data[i][j];
        }
    }

    return result;
}


Matrix Matrix::operator-(const Matrix& other) {

    if (rows != other.rows || cols != other.cols) {
        throw MatrixError("Haiyaa!! Substraction Failure dimensions should be same");
    }

    Matrix result(rows, cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result.data[i][j] = data[i][j] - other.data[i][j];
        }
    }

    return result;
}
