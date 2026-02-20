#include "../include/Matrix.hpp"
#include "../utils/Input.hpp"
#include "../utils/Display.hpp"
#include <cmath>
#include <iostream>
using namespace std;


GaussianElimination::GaussianElimination(int size) {
    this->n = size;

    mat = new double*[n];
    for (int i = 0; i < n; i++) {
        mat[i] = new double[n + 1];
        for (int j = 0; j <= n; j++) {
            mat[i][j] = 0.0;
        }
    }

    solution = new double[n];
    for (int i = 0; i < n; i++) {
        solution[i] = 0.0;
    }
}


GaussianElimination::~GaussianElimination() {
    for (int i = 0; i < n; i++) {
        delete[] mat[i];
    }
    delete[] mat;
    delete[] solution;
}


void GaussianElimination::takeInput() {
    takeGaussInput(mat, n);
}


void GaussianElimination::compute() {
    try {


        for (int col = 0; col < n; col++) {

            if (fabs(mat[col][col]) < 0.000000000000001) {
                throw MatrixError("Zero on diagonal — system can't be solved this way.");
            }

            for (int row = col + 1; row < n; row++) {
                double factor = mat[row][col] / mat[col][col];
                for (int j = col; j <= n; j++) {
                    mat[row][j] -= factor * mat[col][j];
                }
            }
        }


        for (int i = n - 1; i >= 0; i--) {
            solution[i] = mat[i][n];
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


void GaussianElimination::showSolution() {
    ::showSolution(solution, n);
}
