#include "../include/Matrix.hpp"
#include "../utils/Input.hpp"
#include "../utils/Display.hpp"
#include <cmath>     // for fabs()
#include <iostream>
using namespace std;


// Constructor — set up the augmented matrix [A|b]
// Example for 2 equations: stores a 2x3 grid
GaussianElimination::GaussianElimination(int size) {
    this->n = size;   // this-> used in constructor

    mat = new double*[n];
    for (int i = 0; i < n; i++) {
        mat[i] = new double[n + 1];   // n coefficients + 1 answer
        for (int j = 0; j <= n; j++) {
            mat[i][j] = 0.0;
        }
    }

    solution = new double[n];
    for (int i = 0; i < n; i++) {
        solution[i] = 0.0;
    }
}


// Destructor — free the memory
GaussianElimination::~GaussianElimination() {
    for (int i = 0; i < n; i++) {
        delete[] mat[i];
    }
    delete[] mat;
    delete[] solution;
}


// takeInput — asks user to fill the augmented matrix
void GaussianElimination::takeInput() {
    takeGaussInput(mat, n);   // from utils/input.cpp
}


// compute — does forward elimination then back substitution
void GaussianElimination::compute() {
    try {

        // --- Step 1: Forward Elimination ---
        // Turn the matrix into upper triangular form (zeros below diagonal)
        for (int col = 0; col < n; col++) {

            // if the diagonal is 0 we can't divide — throw an error
            if (fabs(mat[col][col]) < 1e-9) {
                throw MatrixError("Zero on diagonal — system can't be solved this way.");
            }

            for (int row = col + 1; row < n; row++) {
                double factor = mat[row][col] / mat[col][col];
                for (int j = col; j <= n; j++) {
                    mat[row][j] -= factor * mat[col][j];
                }
            }
        }

        // --- Step 2: Back Substitution ---
        // Solve from the last equation upward
        for (int i = n - 1; i >= 0; i--) {
            solution[i] = mat[i][n];   // start with the RHS value
            for (int j = i + 1; j < n; j++) {
                solution[i] -= mat[i][j] * solution[j];
            }
            solution[i] /= mat[i][i];
        }

        cout << "\nComputation done!" << endl;

    }
    catch (MatrixError& e) {
        cout << "Error during Gaussian Elimination: " << e.what() << endl;
    }
}


// showSolution — prints x1, x2, x3...
void GaussianElimination::showSolution() {
    ::showSolution(solution, n);   // from utils/display.cpp
}