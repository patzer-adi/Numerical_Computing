#include "../include/Integration.hpp"
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

// number of columns in the results table
// col 0: func_index, col 1: n, col 2: exact, col 3: approx, col 4: error
static const int NUM_COLS = 5;

// default constructor — start with room for 10 functions
Integration::Integration() : Matrix() {
    maxFunctions = 10;
    numFunctions = 0;
    functions = new IntegrationFunctionEntry[maxFunctions];

    subIntervalCounts = nullptr;
    numN = 0;

    functionNames = nullptr;
}

// destructor — clean up our own allocations
Integration::~Integration() {
    delete[] functions;
    if (subIntervalCounts != nullptr)
        delete[] subIntervalCounts;
    if (functionNames != nullptr)
        delete[] functionNames;
}

// register an integrand with its known exact integral
void Integration::addFunction(string name, IntegrandFunction f, ExactIntegralFunction F) {
    // grow array if needed
    if (numFunctions >= maxFunctions) {
        int newMax = maxFunctions * 2;
        IntegrationFunctionEntry *newArr = new IntegrationFunctionEntry[newMax];
        for (int i = 0; i < numFunctions; i++)
            newArr[i] = functions[i];
        delete[] functions;
        functions = newArr;
        maxFunctions = newMax;
    }

    functions[numFunctions].name = name;
    functions[numFunctions].f = f;
    functions[numFunctions].F = F;
    numFunctions++;
}

// set sub-interval counts to evaluate at — copies the array
void Integration::setSubIntervals(int *n, int count) {
    if (subIntervalCounts != nullptr)
        delete[] subIntervalCounts;

    numN = count;
    subIntervalCounts = new int[numN];
    for (int i = 0; i < numN; i++)
        subIntervalCounts[i] = n[i];
}

// getters
int Integration::getNumFunctions() const { return numFunctions; }
int Integration::getNumN() const { return numN; }

IntegrationFunctionEntry Integration::getFunction(int i) const {
    if (i < 0 || i >= numFunctions)
        throw MatrixException("function index out of bounds");
    return functions[i];
}

int Integration::getN(int i) const {
    if (i < 0 || i >= numN)
        throw MatrixException("n index out of bounds");
    return subIntervalCounts[i];
}

// ===== COMPUTE ALL =====

// compute the integral for all registered functions at all n values over [a, b]
// uses the virtual integrate() — each derived class provides its formula
//
// Matrix layout: (numFunctions * numN) rows × 5 columns
// Col 0: func_index  Col 1: n  Col 2: exact  Col 3: approx  Col 4: error
void Integration::computeAll(double a, double b) {
    if (numFunctions < 1)
        throw MatrixException("no functions registered... add some first");
    if (numN < 1)
        throw MatrixException("no sub-interval counts set... call setSubIntervals() first");
    if (a >= b)
        throw MatrixException("invalid integration bounds: a must be less than b");

    int totalRows = numFunctions * numN;

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
        IntegrandFunction f = functions[fi].f;
        ExactIntegralFunction F = functions[fi].F;
        double exact = F(a, b);

        for (int ni = 0; ni < numN; ni++) {
            int n = subIntervalCounts[ni];

            double approx = integrate(f, a, b, n);

            data[row][0] = (double)fi;              // func index
            data[row][1] = (double)n;               // sub-intervals
            data[row][2] = exact;                    // exact integral
            data[row][3] = approx;                   // approximation
            data[row][4] = fabs(exact - approx);     // absolute error

            functionNames[row] = functions[fi].name;
            row++;
        }
    }

    cout << getMethodName() << ": computed for " << numFunctions
         << " functions × " << numN << " sub-interval counts (" << totalRows
         << " rows)" << endl;
}

// ===== DISPLAY =====

// display results as a nicely formatted table
void Integration::display() const {
    if (rows == 0 || cols == 0) {
        cout << "No results computed yet... call computeAll() first" << endl;
        return;
    }

    cout << "\n=== " << getMethodName() << " ===" << endl;

    // column headers
    cout << left << setw(18) << "Function"
         << right
         << setw(8)  << "n"
         << setw(16) << "Exact"
         << setw(16) << "Approx"
         << setw(16) << "Error"
         << endl;

    // separator line
    cout << string(18 + 8 + 16 * 3, '-') << endl;

    cout << scientific << setprecision(6);

    for (int i = 0; i < rows; i++) {
        string fname = (functionNames != nullptr) ? functionNames[i] : "?";
        cout << left << setw(18) << fname
             << right
             << fixed << setprecision(0) << setw(8) << data[i][1]
             << scientific << setprecision(6)
             << setw(16) << data[i][2]
             << setw(16) << data[i][3]
             << setw(16) << data[i][4]
             << endl;
    }
    cout << endl;
}

// ===== SAVE RESULTS =====

// save results to file in scientific notation
void Integration::saveResults(string filename) const {
    if (rows == 0 || cols == 0)
        throw MatrixException("no results to save... call computeAll() first");

    ofstream fout(filename);
    if (!fout)
        throw MatrixException("can't write to file '" + filename + "'");

    // header
    fout << "# " << getMethodName() << endl;
    fout << left << setw(18) << "Function"
         << right
         << setw(8)  << "n"
         << setw(16) << "Exact"
         << setw(16) << "Approx"
         << setw(16) << "Error"
         << "\n";

    for (int i = 0; i < rows; i++) {
        string fname = (functionNames != nullptr) ? functionNames[i] : "?";
        fout << left << setw(18) << fname
             << right
             << fixed << setprecision(0) << setw(8) << data[i][1]
             << scientific << setprecision(8)
             << setw(16) << data[i][2]
             << setw(16) << data[i][3]
             << setw(16) << data[i][4]
             << "\n";
    }

    fout.close();
    cout << "Results saved to " << filename << endl;
}
