#include "Input.hpp"
#include "../include/MatrixException.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

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
                          "'... did you spell it right? ");
  }

  // try reading first line to see if it has dimensions
  string firstLine;
  getline(fin, firstLine);
  istringstream iss(firstLine);

  vector<double> firstRow;
  double val;
  while (iss >> val) {
    firstRow.push_back(val);
  }

  // now read rest of file
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
    // first line is dimensions header
    rows = (int)firstRow[0];
    cols = (int)firstRow[1];
  } else {
    // first line is actual data
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
}

// read RHS vector from file (one value per line)
void readRHSFromFile(string filename, double *&b, int &n) {
  ifstream fin(filename);
  if (!fin) {
    throw MatrixException("can't open RHS file '" + filename +
                          "'... file said bye bye 👋");
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

// read augmented matrix [A|b] from file, split into A and b
void readAugmentedFromFile(string filename, double **&A, double *&b, int &n) {
  double **augmented = nullptr;
  int augRows, augCols;
  readMatrixFromFile(filename, augmented, augRows, augCols);

  n = augRows;
  int aCols = augCols - 1; // last column is b

  // allocate A
  A = new double *[n];
  for (int i = 0; i < n; i++) {
    A[i] = new double[aCols];
    for (int j = 0; j < aCols; j++)
      A[i][j] = augmented[i][j];
  }

  // last column is b
  b = new double[n];
  for (int i = 0; i < n; i++) {
    b[i] = augmented[i][augCols - 1];
  }

  // cleanup augmented
  for (int i = 0; i < augRows; i++)
    delete[] augmented[i];
  delete[] augmented;
}

// guess file type from name
// looks for 'l', 'left' = matrix, 'r', 'right' = vector
char guessFileType(string filename) {
  // just check the filename for hints
  string lower = filename;
  for (int i = 0; i < (int)lower.size(); i++) {
    if (lower[i] >= 'A' && lower[i] <= 'Z')
      lower[i] = lower[i] + 32;
  }

  if (lower.find("right") != string::npos || lower.find("r.") != string::npos)
    return 'r';
  if (lower.find("left") != string::npos || lower.find("l.") != string::npos)
    return 'l';

  // default: try to guess from dimensions - if cols = rows+1 its augmented
  return 'a'; // assume augmented
}

// interactive: get matrix input from user
void getMatrixInput(double **&data, int &rows, int &cols) {
  int choice;
  cout << "\nHow do you want to enter the matrix?\n";
  cout << "1. Enter manually\n";
  cout << "2. Load from file\n";
  cout << "Enter choice: ";
  cin >> choice;

  if (choice == 1) {
    cout << "Enter rows: ";
    cin >> rows;
    cout << "Enter cols: ";
    cin >> cols;
    data = new double *[rows];
    for (int i = 0; i < rows; i++)
      data[i] = new double[cols];
    readMatrixFromConsole(data, rows, cols);
  } else if (choice == 2) {
    string filename;
    cout << "Enter filename: ";
    cin >> filename;
    readMatrixFromFile(filename, data, rows, cols);
    cout << "Loaded " << rows << "x" << cols << " matrix from " << filename
         << endl;
  } else {
    throw MatrixException("invalid choice bruh... it was literally 1 or 2 💀");
  }
}

// interactive: get system of equations (A and b)
void getSystemInput(double **&A, double *&b, int &n) {
  int choice;
  cout << "\nHow do you want to input the system Ax = b?\n";
  cout << "1. Enter A and b manually\n";
  cout << "2. Load from augmented file [A|b]\n";
  cout << "3. Load A from left file and b from right file\n";
  cout << "Enter choice: ";
  cin >> choice;

  if (choice == 1) {
    cout << "Enter size n (nxn matrix): ";
    cin >> n;
    A = new double *[n];
    for (int i = 0; i < n; i++)
      A[i] = new double[n];
    cout << "\n--- Enter matrix A ---\n";
    readMatrixFromConsole(A, n, n);
    b = new double[n];
    cout << "\n--- Enter vector b ---\n";
    cout << "Enter " << n << " values: ";
    for (int i = 0; i < n; i++)
      cin >> b[i];
  } else if (choice == 2) {
    string filename;
    cout << "Enter augmented matrix file: ";
    cin >> filename;
    readAugmentedFromFile(filename, A, b, n);
    cout << "Loaded " << n << "x" << n << " system from " << filename << endl;
  } else if (choice == 3) {
    string leftFile, rightFile;
    cout << "Enter matrix (left) file: ";
    cin >> leftFile;
    cout << "Enter RHS (right) file: ";
    cin >> rightFile;

    double **tempData = nullptr;
    int tempRows, tempCols;
    readMatrixFromFile(leftFile, tempData, tempRows, tempCols);

    // figure out if this file is augmented or just A
    if (tempCols == tempRows + 1) {
      // its augmented, extract A and b
      n = tempRows;
      A = new double *[n];
      for (int i = 0; i < n; i++) {
        A[i] = new double[n];
        for (int j = 0; j < n; j++)
          A[i][j] = tempData[i][j];
      }
      // but user also gave right file, so use that for b
      int bSize;
      readRHSFromFile(rightFile, b, bSize);
      if (bSize != n)
        throw MatrixException(
            "RHS size doesn't match matrix size... they broke up 💔");
    } else {
      n = tempRows;
      A = new double *[n];
      for (int i = 0; i < n; i++) {
        A[i] = new double[n];
        for (int j = 0; j < n; j++)
          A[i][j] = tempData[i][j];
      }
      int bSize;
      readRHSFromFile(rightFile, b, bSize);
      if (bSize != n)
        throw MatrixException("Matrix and RHS sizes don't match... awkward 😅");
    }

    // cleanup temp
    for (int i = 0; i < tempRows; i++)
      delete[] tempData[i];
    delete[] tempData;

    cout << "Loaded " << n << "x" << n << " system\n";
  } else {
    throw MatrixException("bro that wasn't even an option 😐");
  }
}
