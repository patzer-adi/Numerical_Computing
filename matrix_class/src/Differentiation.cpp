#include "../include/Differentiation.hpp"
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

// number of columns in the results table
// col 0: func_index, col 1: h, col 2: exact, col 3: approx, col 4: error
static const int NUM_COLS = 5;

// default constructor — start with room for 10 functions
Differentiation::Differentiation() : Matrix() {
    maxFunctions = 10;
    numFunctions = 0;
    functions = new FunctionEntry[maxFunctions];

    hValues = nullptr;
    numH = 0;

    functionNames = nullptr;
}

// destructor — clean up our own allocations
Differentiation::~Differentiation() {
    delete[] functions;
    if (hValues != nullptr)
        delete[] hValues;
    if (functionNames != nullptr)
        delete[] functionNames;
}

// register a function with its known derivative
void Differentiation::addFunction(string name, MathFunction f, MathFunction df) {
    // grow array if needed
    if (numFunctions >= maxFunctions) {
        int newMax = maxFunctions * 2;
        FunctionEntry *newArr = new FunctionEntry[newMax];
        for (int i = 0; i < numFunctions; i++)
            newArr[i] = functions[i];
        delete[] functions;
        functions = newArr;
        maxFunctions = newMax;
    }

    functions[numFunctions].name = name;
    functions[numFunctions].f = f;
    functions[numFunctions].df = df;
    numFunctions++;
}

// set step sizes to evaluate at — copies the array
void Differentiation::setStepSizes(double *h, int n) {
    if (hValues != nullptr)
        delete[] hValues;

    numH = n;
    hValues = new double[numH];
    for (int i = 0; i < numH; i++)
        hValues[i] = h[i];
}

// getters
int Differentiation::getNumFunctions() const { return numFunctions; }
int Differentiation::getNumH() const { return numH; }

FunctionEntry Differentiation::getFunction(int i) const {
    if (i < 0 || i >= numFunctions)
        throw MatrixException("function index out of bounds");
    return functions[i];
}

double Differentiation::getH(int i) const {
    if (i < 0 || i >= numH)
        throw MatrixException("h index out of bounds");
    return hValues[i];
}

// ===== COMPUTE ALL =====

// compute the derivative for all registered functions at all h values
// uses the virtual computeDerivative() — each derived class provides its formula
//
// Matrix layout: (numFunctions * numH) rows × 5 columns
// Col 0: func_index  Col 1: h  Col 2: exact  Col 3: approx  Col 4: error
void Differentiation::computeAll(double x0) {
    if (numFunctions < 1)
        throw MatrixException("no functions registered... add some first");
    if (numH < 1)
        throw MatrixException("no step sizes set... call setStepSizes() first");

    int totalRows = numFunctions * numH;

    // clean up old Matrix data if any
    if (data != nullptr) {
        for (int i = 0; i < rows; i++)
            delete[] data[i];
        delete[] data;
    }

    // allocate the results matrix
    rows = totalRows;
    cols = NUM_COLS;
    data = new double *[rows];
    for (int i = 0; i < rows; i++) {
        data[i] = new double[cols];
        for (int j = 0; j < cols; j++)
            data[i][j] = 0.0;
    }

    // allocate function names array for display (one name per row)
    if (functionNames != nullptr)
        delete[] functionNames;
    functionNames = new string[totalRows];

    // fill in the results
    int row = 0;
    for (int fi = 0; fi < numFunctions; fi++) {
        MathFunction f = functions[fi].f;
        MathFunction df = functions[fi].df;
        double exact = df(x0);

        for (int hi = 0; hi < numH; hi++) {
            double h = hValues[hi];

            double approx = computeDerivative(f, x0, h);

            data[row][0] = (double)fi;              // func index
            data[row][1] = h;                        // step size
            data[row][2] = exact;                    // exact derivative
            data[row][3] = approx;                   // approximation
            data[row][4] = fabs(exact - approx);     // absolute error

            functionNames[row] = functions[fi].name;
            row++;
        }
    }

    cout << getMethodName() << ": computed for " << numFunctions
         << " functions × " << numH << " step sizes (" << totalRows
         << " rows)" << endl;
}

// ===== DISPLAY =====

// display results as a nicely formatted table
void Differentiation::display() const {
    if (rows == 0 || cols == 0) {
        cout << "No results computed yet... call computeAll() first" << endl;
        return;
    }

    cout << "\n=== " << getMethodName() << " ===" << endl;

    // column headers
    cout << left << setw(18) << "Function"
         << right
         << setw(14) << "h"
         << setw(16) << "Exact"
         << setw(16) << "Approx"
         << setw(16) << "Error"
         << endl;

    // separator line
    cout << string(18 + 14 + 16 * 3, '-') << endl;

    cout << scientific << setprecision(6);

    for (int i = 0; i < rows; i++) {
        string fname = (functionNames != nullptr) ? functionNames[i] : "?";
        cout << left << setw(18) << fname
             << right
             << setw(14) << data[i][1]
             << setw(16) << data[i][2]
             << setw(16) << data[i][3]
             << setw(16) << data[i][4]
             << endl;
    }
    cout << endl;
}

// ===== SAVE RESULTS =====

// save results to file in scientific notation
void Differentiation::saveResults(string filename) const {
    if (rows == 0 || cols == 0)
        throw MatrixException("no results to save... call computeAll() first");

    ofstream fout(filename);
    if (!fout)
        throw MatrixException("can't write to file '" + filename + "'");

    // header
    fout << "# " << getMethodName() << endl;
    fout << left << setw(18) << "Function"
         << right
         << setw(14) << "h"
         << setw(16) << "Exact"
         << setw(16) << "Approx"
         << setw(16) << "Error"
         << "\n";

    fout << scientific << setprecision(8);

    for (int i = 0; i < rows; i++) {
        string fname = (functionNames != nullptr) ? functionNames[i] : "?";
        fout << left << setw(18) << fname
             << right
             << setw(14) << data[i][1]
             << setw(16) << data[i][2]
             << setw(16) << data[i][3]
             << setw(16) << data[i][4]
             << "\n";
    }

    fout.close();
    cout << "Results saved to " << filename << endl;
}
