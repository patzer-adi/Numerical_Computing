#include "../include/Matrix.hpp"
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#ifdef USE_CUDA
#include "../cuda/include/gpu_backend.cuh"
#include "../cuda/include/gpu_dispatch.cuh"
#endif

using namespace std;

// default constructor
Matrix::Matrix() {
  rows = 0;
  cols = 0;
  data = nullptr;
}

// parameterized constructor
Matrix::Matrix(int r, int c) {
  rows = r;
  cols = c;
  data = new double *[rows];
  for (int i = 0; i < rows; i++) {
    data[i] = new double[cols];
    for (int j = 0; j < cols; j++) {
      data[i][j] = 0.0;
    }
  }
}

// file constructor — reads matrix from a file
Matrix::Matrix(string filename) {
  data = nullptr;
  rows = 0;
  cols = 0;
  readFromFile(filename);
}

// copy constructor
Matrix::Matrix(const Matrix &other) {
  rows = other.rows;
  cols = other.cols;
  data = new double *[rows];
  for (int i = 0; i < rows; i++) {
    data[i] = new double[cols];
    for (int j = 0; j < cols; j++) {
      data[i][j] = other.data[i][j];
    }
  }
}

// destructor
Matrix::~Matrix() {
  if (data != nullptr) {
    for (int i = 0; i < rows; i++) {
      delete[] data[i];
    }
    delete[] data;
  }
}

// getters
int Matrix::getRows() { return rows; }
int Matrix::getCols() { return cols; }

double Matrix::getData(int i, int j) {
  if (i < 0 || i >= rows || j < 0 || j >= cols)
    throw MatrixException(
        "bruh you went out of bounds... matrix ain't that big");
  return data[i][j];
}

void Matrix::setData(int i, int j, double val) {
  if (i < 0 || i >= rows || j < 0 || j >= cols)
    throw MatrixException(
        "you're trying to set data outside the matrix... nice try");
  data[i][j] = val;
}

// ===== I/O METHODS =====

// read matrix from console — ask for size, then row by row
void Matrix::readFromConsole() {
  // clean up old data if any
  if (data != nullptr) {
    for (int i = 0; i < rows; i++)
      delete[] data[i];
    delete[] data;
  }

  cout << "Enter rows: ";
  cin >> rows;
  cout << "Enter cols: ";
  cin >> cols;

  data = new double *[rows];
  for (int i = 0; i < rows; i++)
    data[i] = new double[cols];

  cout << "Enter matrix elements row by row:\n";
  for (int i = 0; i < rows; i++) {
    cout << "Row " << i + 1 << ": ";
    for (int j = 0; j < cols; j++) {
      cin >> data[i][j];
    }
  }
}

// read matrix from a file (auto detects header vs no header)
void Matrix::readFromFile(string filename) {
  // clean up old data if any
  if (data != nullptr) {
    for (int i = 0; i < rows; i++)
      delete[] data[i];
    delete[] data;
    data = nullptr;
  }

  ifstream fin(filename);
  if (!fin) {
    throw MatrixException("can't open file '" + filename +
                          "'... did you spell it right?");
  }

  // read first line to check if its a header
  string firstLine;
  getline(fin, firstLine);
  istringstream iss(firstLine);

  vector<double> firstRow;
  double val;
  while (iss >> val) {
    firstRow.push_back(val);
  }

  // read remaining lines
  vector<vector<double>> allRows;
  string line;
  while (getline(fin, line)) {
    if (line.empty())
      continue;
    istringstream lineStream(line);
    vector<double> row;
    double v;
    while (lineStream >> v) {
      row.push_back(v);
    }
    if (!row.empty())
      allRows.push_back(row);
  }
  fin.close();

  // check if first line was "rows cols" header
  if (firstRow.size() == 2 && !allRows.empty() && allRows[0].size() > 2) {
    rows = (int)firstRow[0];
    cols = (int)firstRow[1];
  } else {
    allRows.insert(allRows.begin(), firstRow);
    rows = allRows.size();
    cols = allRows[0].size();
  }

  // allocate and fill
  data = new double *[rows];
  for (int i = 0; i < rows; i++) {
    data[i] = new double[cols];
    for (int j = 0; j < cols; j++) {
      if (i < (int)allRows.size() && j < (int)allRows[i].size())
        data[i][j] = allRows[i][j];
      else
        data[i][j] = 0.0;
    }
  }

  cout << "Loaded " << rows << "x" << cols << " matrix from " << filename
       << endl;
}

// write matrix to a file
void Matrix::saveToFile(string filename) {
  ofstream fout(filename);
  if (!fout) {
    throw MatrixException("can't write to file '" + filename + "'");
  }
  fout << fixed << setprecision(7);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (j > 0)
        fout << "  ";
      fout << data[i][j];
    }
    fout << "\n";
  }
  fout.close();
  cout << "Matrix written to " << filename << endl;
}

// static helper — asks user "manual or file?" and returns a Matrix
Matrix Matrix::inputMatrix(string label) {
  cout << "\n--- " << label << " ---\n";
  int choice;
  cout << "How do you want to enter the matrix?\n";
  cout << "1. Enter manually\n";
  cout << "2. Load from file\n";
  cout << "Enter choice: ";
  cin >> choice;

  Matrix m;
  if (choice == 1) {
    m.readFromConsole();
  } else if (choice == 2) {
    string filename;
    cout << "Enter filename: ";
    cin >> filename;
    m.readFromFile(filename);
  } else {
    throw MatrixException("invalid choice bruh... it was literally 1 or 2");
  }
  return m;
}

// ===== OPERATIONS =====

// copy from another matrix
void Matrix::copyFrom(Matrix &other) {
  if (data != nullptr) {
    for (int i = 0; i < rows; i++)
      delete[] data[i];
    delete[] data;
  }
  rows = other.rows;
  cols = other.cols;
  data = new double *[rows];
  for (int i = 0; i < rows; i++) {
    data[i] = new double[cols];
    for (int j = 0; j < cols; j++)
      data[i][j] = other.data[i][j];
  }
}

// assignment operator (deep copy)
Matrix &Matrix::operator=(const Matrix &other) {
  if (this == &other)
    return *this;
  if (data != nullptr) {
    for (int i = 0; i < rows; i++)
      delete[] data[i];
    delete[] data;
  }
  rows = other.rows;
  cols = other.cols;
  data = new double *[rows];
  for (int i = 0; i < rows; i++) {
    data[i] = new double[cols];
    for (int j = 0; j < cols; j++)
      data[i][j] = other.data[i][j];
  }
  return *this;
}

// addition
Matrix Matrix::add(Matrix other) {
  if (rows != other.rows || cols != other.cols)
    throw MatrixException(
        "can't add these matrices... they don't even match bro");
  Matrix result(rows, cols);

#ifdef USE_CUDA
  if (BackendDispatcher::shouldUseGPU(rows, "matadd")) {
    int total = rows * cols;
    double *hostA = new double[total];
    double *hostB = new double[total];
    double *hostC = new double[total];
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++) {
        hostA[i * cols + j] = data[i][j];
        hostB[i * cols + j] = other.data[i][j];
      }
    gpuMatAdd(hostA, hostB, hostC, rows, cols);
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        result.data[i][j] = hostC[i * cols + j];
    delete[] hostA;
    delete[] hostB;
    delete[] hostC;
    return result;
  }
#endif

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      result.data[i][j] = data[i][j] + other.data[i][j];
  return result;
}

Matrix Matrix::operator+(const Matrix &other) {
  Matrix temp(other);
  return add(temp);
}

// subtraction
Matrix Matrix::subtract(Matrix other) {
  if (rows != other.rows || cols != other.cols)
    throw MatrixException(
        "subtraction needs same size matrices... this ain't it chief");
  Matrix result(rows, cols);

#ifdef USE_CUDA
  if (BackendDispatcher::shouldUseGPU(rows, "matadd")) {
    int total = rows * cols;
    double *hostA = new double[total];
    double *hostB = new double[total];
    double *hostC = new double[total];
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++) {
        hostA[i * cols + j] = data[i][j];
        hostB[i * cols + j] = other.data[i][j];
      }
    gpuMatSub(hostA, hostB, hostC, rows, cols);
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        result.data[i][j] = hostC[i * cols + j];
    delete[] hostA;
    delete[] hostB;
    delete[] hostC;
    return result;
  }
#endif

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      result.data[i][j] = data[i][j] - other.data[i][j];
  return result;
}

Matrix Matrix::operator-(const Matrix &other) {
  Matrix temp(other);
  return subtract(temp);
}

// multiplication
Matrix Matrix::multiply(Matrix other) {
  if (cols != other.rows)
    throw MatrixException("matrix multiplication dimensions don't work... go "
                          "back to math class");
  Matrix result(rows, other.cols);

#ifdef USE_CUDA
  if (BackendDispatcher::shouldUseGPU(rows, "matmul")) {
    int totalA = rows * cols;
    int totalB = other.rows * other.cols;
    int totalC = rows * other.cols;
    double *hostA = new double[totalA];
    double *hostB = new double[totalB];
    double *hostC = new double[totalC];
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
        hostA[i * cols + j] = data[i][j];
    for (int i = 0; i < other.rows; i++)
      for (int j = 0; j < other.cols; j++)
        hostB[i * other.cols + j] = other.data[i][j];
    gpuMatMul(hostA, hostB, hostC, rows, cols, other.cols);
    for (int i = 0; i < rows; i++)
      for (int j = 0; j < other.cols; j++)
        result.data[i][j] = hostC[i * other.cols + j];
    delete[] hostA;
    delete[] hostB;
    delete[] hostC;
    return result;
  }
#endif

  for (int i = 0; i < rows; i++)
    for (int j = 0; j < other.cols; j++)
      for (int k = 0; k < cols; k++)
        result.data[i][j] += data[i][k] * other.data[k][j];
  return result;
}

Matrix Matrix::operator*(const Matrix &other) {
  Matrix temp(other);
  return multiply(temp);
}

// determinant using gaussian elimination style
double Matrix::determinant() {
  if (rows != cols)
    throw MatrixException(
        "determinant only works on square matrices... you knew that right?");

  int n = rows;

  // make a copy so we dont mess up the original
  double **temp = new double *[n];
  for (int i = 0; i < n; i++) {
    temp[i] = new double[n];
    for (int j = 0; j < n; j++)
      temp[i][j] = data[i][j];
  }

  double det = 1.0;
  for (int k = 0; k < n; k++) {
    // partial pivoting
    int maxRow = k;
    double maxVal = fabs(temp[k][k]);
    for (int i = k + 1; i < n; i++) {
      if (fabs(temp[i][k]) > maxVal) {
        maxVal = fabs(temp[i][k]);
        maxRow = i;
      }
    }

    if (maxVal < 1e-12) {
      for (int i = 0; i < n; i++)
        delete[] temp[i];
      delete[] temp;
      return 0.0;
    }

    // swap rows if needed
    if (maxRow != k) {
      double *tempRow = temp[k];
      temp[k] = temp[maxRow];
      temp[maxRow] = tempRow;
      det *= -1;
    }

    det *= temp[k][k];

    // eliminate below
    for (int i = k + 1; i < n; i++) {
      double factor = temp[i][k] / temp[k][k];
      for (int j = k; j < n; j++)
        temp[i][j] -= factor * temp[k][j];
    }
  }

  // cleanup
  for (int i = 0; i < n; i++)
    delete[] temp[i];
  delete[] temp;

  return det;
}

// get pointer to row i (for CUDA memory transfer)
double *Matrix::getRowPointer(int i) {
  if (i < 0 || i >= rows)
    throw MatrixException("row index out of bounds in getRowPointer");
  return data[i];
}

// display matrix
void Matrix::display() {
  cout << fixed << setprecision(6);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++)
      cout << setw(14) << data[i][j];
    cout << endl;
  }
  cout << endl;
}
