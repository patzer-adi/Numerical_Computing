#include "../include/Matrix.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>

#ifdef USE_CUDA
#include "../cuda/include/gpu_backend.cuh"
#include "../cuda/include/gpu_dispatch.cuh"
#endif

using namespace std;

// scalar multiplication: every element * scalar
Matrix Matrix::operator*(double scalar) {
  Matrix result(rows, cols);

#ifdef USE_CUDA
  if (BackendDispatcher::shouldUseGPU(rows, "matadd")) {
    int total = rows * cols;
    double *hostA = new double[total];
    double *hostB = new double[total];
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        hostA[i * cols + j] = data[i][j];
    gpuScalarMul(hostA, hostB, scalar, rows, cols);
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        result.data[i][j] = hostB[i * cols + j];
    delete[] hostA;
    delete[] hostB;
    return result;
  }
#endif

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      result.data[i][j] = data[i][j] * scalar;
  return result;
}

// transpose: swap rows and cols
Matrix Matrix::transpose() {
  Matrix result(cols, rows);

#ifdef USE_CUDA
  if (BackendDispatcher::shouldUseGPU(rows, "matadd")) {
    int total = rows * cols;
    double *hostA = new double[total];
    double *hostB = new double[total];
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        hostA[i * cols + j] = data[i][j];
    gpuTranspose(hostA, hostB, rows, cols);
    for (int i = 0; i < cols; i++)
      for (int j = 0; j < rows; j++)
        result.data[i][j] = hostB[i * rows + j];
    delete[] hostA;
    delete[] hostB;
    return result;
  }
#endif

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      result.data[j][i] = data[i][j];
  return result;
}

// minor matrix: remove row r and col c, return (n-1)x(n-1)
Matrix Matrix::minorMatrix(int r, int c) {
  if (rows != cols)
    throw MatrixException(
        "minor matrix only works on square matrices... come on");
  if (rows <= 1)
    throw MatrixException(
        "matrix too small to take a minor... it's already 1x1");

  int n = rows;
  Matrix result(n - 1, n - 1);
  int ri = 0;
  for (int i = 0; i < n; i++) {
    if (i == r)
      continue;
    int ci = 0;
    for (int j = 0; j < n; j++) {
      if (j == c)
        continue;
      result.data[ri][ci] = data[i][j];
      ci++;
    }
    ri++;
  }
  return result;
}

// cofactor: (-1)^(r+c) * det(minor(r,c))
double Matrix::cofactor(int r, int c) {
  Matrix minor = minorMatrix(r, c);
  double det = minor.determinant();
  double sign = ((r + c) % 2 == 0) ? 1.0 : -1.0;
  return sign * det;
}

// adjoint: transpose of the cofactor matrix
Matrix Matrix::adjoint() {
  if (rows != cols)
    throw MatrixException(
        "adjoint only works on square matrices... nice try though");

  int n = rows;
  Matrix cofactorMatrix(n, n);

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      cofactorMatrix.data[i][j] = cofactor(i, j);

  return cofactorMatrix.transpose();
}

// inverse: adj(A) / det(A)
Matrix Matrix::inverse() {
  if (rows != cols)
    throw MatrixException(
        "inverse only works on square matrices... everybody knows that");

  double det = determinant();
  if (fabs(det) < 1e-12)
    throw MatrixException(
        "matrix is singular (det = 0)... inverse doesn't exist, just like my "
        "social life 💀");

  Matrix adj = adjoint();
  double invDet = 1.0 / det;

  return adj * invDet;
}

// ===== STREAM OPERATORS (friend functions) =====

// insertion operator: cout << matrix
ostream &operator<<(ostream &out, const Matrix &m) {
  out << fixed << setprecision(6);
  for (int i = 0; i < m.rows; i++) {
    for (int j = 0; j < m.cols; j++)
      out << setw(14) << m.data[i][j];
    out << endl;
  }
  out << endl;
  return out;
}

// extraction operator: cin >> matrix
istream &operator>>(istream &in, Matrix &m) {
  // clean up old data if any
  if (m.data != nullptr) {
    for (int i = 0; i < m.rows; i++)
      delete[] m.data[i];
    delete[] m.data;
  }

  cout << "Enter rows: ";
  in >> m.rows;
  cout << "Enter cols: ";
  in >> m.cols;

  m.data = new double *[m.rows];
  for (int i = 0; i < m.rows; i++)
    m.data[i] = new double[m.cols];

  cout << "Enter matrix elements row by row:" << endl;
  for (int i = 0; i < m.rows; i++) {
    cout << "Row " << i + 1 << ": ";
    for (int j = 0; j < m.cols; j++) {
      in >> m.data[i][j];
    }
  }
  return in;
}

// ===== ELEMENT ACCESS OPERATOR =====

// access element using A(i, j) syntax
double &Matrix::operator()(int i, int j) {
  if (i < 0 || i >= rows || j < 0 || j >= cols)
    throw MatrixException(
        "bruh you went out of bounds with ()... matrix ain't that big");
  return data[i][j];
}

// ===== EQUALITY OPERATOR =====

// check if two matrices are equal (with floating point tolerance)
bool Matrix::operator==(const Matrix &other) {
  if (rows != other.rows || cols != other.cols)
    return false;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (fabs(data[i][j] - other.data[i][j]) > 1e-10)
        return false;
    }
  }
  return true;
}

// ===== MATRIX CHECK FUNCTIONS =====

// check if matrix is square (rows == cols)
bool Matrix::isSquare() { return rows == cols; }

// check if matrix is symmetric (A[i][j] == A[j][i] for all i,j)
bool Matrix::isSymmetric() {
  if (rows != cols)
    return false;
  for (int i = 0; i < rows; i++) {
    for (int j = i + 1; j < cols; j++) {
      if (fabs(data[i][j] - data[j][i]) > 1e-10)
        return false;
    }
  }
  return true;
}

// check if matrix is identity (diagonal = 1, rest = 0)
bool Matrix::isIdentity() {
  if (rows != cols)
    return false;

#ifdef USE_CUDA
  if (BackendDispatcher::shouldUseGPU(rows, "matadd")) {
    int total = rows * rows;
    double *hostA = new double[total];
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        hostA[i * cols + j] = data[i][j];
    int result = gpuIsIdentity(hostA, rows);
    delete[] hostA;
    return result == 1;
  }
#endif

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (i == j) {
        if (fabs(data[i][j] - 1.0) > 1e-10)
          return false;
      } else {
        if (fabs(data[i][j]) > 1e-10)
          return false;
      }
    }
  }
  return true;
}

// check if matrix is null (all elements are zero)
bool Matrix::isNull() {
#ifdef USE_CUDA
  if (BackendDispatcher::shouldUseGPU(rows, "matadd")) {
    int total = rows * cols;
    double *hostA = new double[total];
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        hostA[i * cols + j] = data[i][j];
    int result = gpuIsNull(hostA, rows, cols);
    delete[] hostA;
    return result == 1;
  }
#endif

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (fabs(data[i][j]) > 1e-10)
        return false;
    }
  }
  return true;
}

// check if matrix is diagonal (only diagonal has non-zero values)
bool Matrix::isDiagonal() {
  if (rows != cols)
    return false;

#ifdef USE_CUDA
  if (BackendDispatcher::shouldUseGPU(rows, "matadd")) {
    int total = rows * rows;
    double *hostA = new double[total];
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        hostA[i * cols + j] = data[i][j];
    int result = gpuIsDiagonal(hostA, rows);
    delete[] hostA;
    return result == 1;
  }
#endif

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (i != j && fabs(data[i][j]) > 1e-10)
        return false;
    }
  }
  return true;
}

// check if matrix is diagonally dominant
// |a[i][i]| >= sum(|a[i][j]|) for j != i, for every row i
bool Matrix::isDiagonallyDominant() {
  if (rows != cols)
    return false;

#ifdef USE_CUDA
  if (BackendDispatcher::shouldUseGPU(rows, "matadd")) {
    int total = rows * rows;
    double *hostA = new double[total];
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        hostA[i * cols + j] = data[i][j];
    int result = gpuIsDiagonallyDominant(hostA, rows);
    delete[] hostA;
    return result == 1;
  }
#endif

  for (int i = 0; i < rows; i++) {
    double diagVal = fabs(data[i][i]);
    double offDiagSum = 0.0;
    for (int j = 0; j < cols; j++) {
      if (j != i)
        offDiagSum += fabs(data[i][j]);
    }
    if (diagVal < offDiagSum)
      return false;
  }
  return true;
}

// try to make matrix diagonally dominant by swapping rows
// returns a new matrix with rows rearranged
Matrix Matrix::makeDiagonallyDominant() {
  if (rows != cols)
    throw MatrixException(
        "can only make square matrices diagonally dominant... come on");

  int n = rows;
  Matrix result(*this); // copy the matrix

  // for each column, find which row has the largest value in that column
  // and swap it to the diagonal position
  for (int col = 0; col < n; col++) {
    // find the row with the max absolute value in this column
    int bestRow = col;
    double bestVal = fabs(result.data[col][col]);

    for (int row = col; row < n; row++) {
      if (fabs(result.data[row][col]) > bestVal) {
        bestVal = fabs(result.data[row][col]);
        bestRow = row;
      }
    }

    // swap rows if needed
    if (bestRow != col) {
      double *temp = result.data[col];
      result.data[col] = result.data[bestRow];
      result.data[bestRow] = temp;
    }
  }

  // check if we actually achieved diagonal dominance
  if (result.isDiagonallyDominant()) {
    cout << "successfully made the matrix diagonally dominant!" << endl;
  } else {
    cout << "tried my best but this matrix can't be made diagonally dominant "
            "just by swapping rows... sorry bro"
         << endl;
  }

  return result;
}
