#include "Input.hpp"
#include "../include/MatrixException.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

// === Low-level functions (unchanged) ===

// read matrix from console
void readMatrixFromConsole(double **data, int rows, int cols) {
  cout << "Enter matrix elements row by row:\n";
  for (int i = 0; i < rows; i++) {
    cout << "Row " << i + 1 << ": ";
    for (int j = 0; j < cols; j++) {
      cin >> data[i][j];
    }
  }
}

// read matrix from file
void readMatrixFromFile(string filename, double **&data, int &rows, int &cols) {
  ifstream fin(filename);
  if (!fin) {
    throw MatrixException("can't open file '" + filename +
                          "'... did you spell it right?");
  }

  string firstLine;
  getline(fin, firstLine);
  istringstream iss(firstLine);

  vector<double> firstRow;
  double val;
  while (iss >> val) {
    firstRow.push_back(val);
  }

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

  if (firstRow.size() == 2 && !allRows.empty() && allRows[0].size() > 2) {
    rows = (int)firstRow[0];
    cols = (int)firstRow[1];
  } else {
    allRows.insert(allRows.begin(), firstRow);
    rows = allRows.size();
    cols = allRows[0].size();
  }

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
}

// read RHS vector from file
void readRHSFromFile(string filename, double *&b, int &n) {
  ifstream fin(filename);
  if (!fin) {
    throw MatrixException("can't open RHS file '" + filename +
                          "'... file said bye bye");
  }

  vector<double> values;
  double val;
  while (fin >> val) {
    values.push_back(val);
  }
  fin.close();

  n = values.size();
  b = new double[n];
  for (int i = 0; i < n; i++) {
    b[i] = values[i];
  }
}

// read augmented [A|b] from file
void readAugmentedFromFile(string filename, double **&A, double *&b, int &n) {
  double **augmented = nullptr;
  int augRows, augCols;
  readMatrixFromFile(filename, augmented, augRows, augCols);

  n = augRows;
  int aCols = augCols - 1;

  A = new double *[n];
  for (int i = 0; i < n; i++) {
    A[i] = new double[aCols];
    for (int j = 0; j < aCols; j++)
      A[i][j] = augmented[i][j];
  }

  b = new double[n];
  for (int i = 0; i < n; i++) {
    b[i] = augmented[i][augCols - 1];
  }

  for (int i = 0; i < augRows; i++)
    delete[] augmented[i];
  delete[] augmented;
}

// guess file type from name
char guessFileType(string filename) {
  string lower = filename;
  for (int i = 0; i < (int)lower.size(); i++) {
    if (lower[i] >= 'A' && lower[i] <= 'Z')
      lower[i] = lower[i] + 32;
  }

  if (lower.find("right") != string::npos || lower.find("r.") != string::npos)
    return 'r';
  if (lower.find("left") != string::npos || lower.find("l.") != string::npos)
    return 'l';

  return 'a';
}

// === High-level class-based functions ===

// asks user "manual or file?" and populates a Matrix object
void getMatrixInput(Matrix &mat) {
  int choice;
  cout << "\nHow do you want to enter the matrix?\n";
  cout << "1. Enter manually\n";
  cout << "2. Load from file\n";
  cout << "Enter choice: ";
  cin >> choice;

  if (choice == 1) {
    mat.readFromConsole();
  } else if (choice == 2) {
    string filename;
    cout << "Enter filename: ";
    cin >> filename;
    mat.readFromFile(filename);
  } else {
    throw MatrixException("invalid choice bruh... it was literally 1 or 2");
  }
}

// asks user how to input Ax=b, populates Matrix A and vector b
void getSystemInput(Matrix &A, double *&b, int &n) {
  int choice;
  cout << "\nHow do you want to input the system Ax = b?\n";
  cout << "1. Enter A and b manually\n";
  cout << "2. Load from augmented file [A|b]\n";
  cout << "3. Load A from left file and b from right file\n";
  cout << "Enter choice: ";
  cin >> choice;

  if (choice == 1) {
    // manual input
    cout << "Enter size n (nxn matrix): ";
    cin >> n;

    // read matrix A
    A = Matrix(n, n);
    cout << "\n--- Enter matrix A ---\n";
    cout << "Enter matrix elements row by row:\n";
    for (int i = 0; i < n; i++) {
      cout << "Row " << i + 1 << ": ";
      for (int j = 0; j < n; j++) {
        double val;
        cin >> val;
        A.setData(i, j, val);
      }
    }

    // read vector b
    b = new double[n];
    cout << "\n--- Enter vector b ---\n";
    cout << "Enter " << n << " values: ";
    for (int i = 0; i < n; i++)
      cin >> b[i];

  } else if (choice == 2) {
    // augmented file
    string filename;
    cout << "Enter augmented matrix file: ";
    cin >> filename;
    loadSystemAugmented(A, b, n, filename);

  } else if (choice == 3) {
    // separate files
    string leftFile, rightFile;
    cout << "Enter matrix (left) file: ";
    cin >> leftFile;
    cout << "Enter RHS (right) file: ";
    cin >> rightFile;
    loadSystemFromFiles(A, b, n, leftFile, rightFile);

  } else {
    throw MatrixException("bro that wasn't even an option");
  }
}

// load A from matrix file, b from rhs file
void loadSystemFromFiles(Matrix &A, double *&b, int &n, string matrixFile,
                         string rhsFile) {
  // read matrix into A
  A.readFromFile(matrixFile);

  n = A.getRows();

  // if file had extra column (NxN+1), trim to square
  if (A.getCols() == n + 1) {
    Matrix trimmed(n, n);
    for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
        trimmed.setData(i, j, A.getData(i, j));
    A = trimmed;
  }

  // read b from file
  int bSize;
  readRHSFromFile(rhsFile, b, bSize);
  if (bSize != n)
    throw MatrixException(
        "RHS size doesn't match matrix size... they broke up");

  cout << "Loaded " << n << "x" << n << " system\n";
}

// load augmented [A|b] from one file
void loadSystemAugmented(Matrix &A, double *&b, int &n, string filename) {
  // read the full file into a temp matrix
  Matrix full;
  full.readFromFile(filename);

  n = full.getRows();
  int totalCols = full.getCols();

  // extract A (first n columns) and b (last column)
  A = Matrix(n, n);
  b = new double[n];
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++)
      A.setData(i, j, full.getData(i, j));
    b[i] = full.getData(i, totalCols - 1);
  }

  cout << "Loaded " << n << "x" << n << " system from " << filename << endl;
}
