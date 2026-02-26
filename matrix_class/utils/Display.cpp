#include "Display.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

void displayMatrix(double **data, int rows, int cols) {
  cout << fixed << setprecision(6);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++)
      cout << setw(14) << data[i][j];
    cout << endl;
  }
  cout << endl;
}

void displaySolution(double *x, int n) {
  cout << "\n--- Solution ---\n";
  cout << fixed << setprecision(6);
  for (int i = 0; i < n; i++)
    cout << "x[" << i << "] = " << x[i] << endl;
  cout << endl;
}

void writeSolutionToFile(double *x, int n, string filename) {
  ofstream fout(filename);
  if (!fout) {
    cout << "could not open file for writing... oh well\n";
    return;
  }
  fout << fixed << setprecision(6);
  for (int i = 0; i < n; i++)
    fout << x[i] << endl;
  fout.close();
  cout << "Solution written to " << filename << endl;
}
