#include "../include/Differentiation.hpp"
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

// number of columns in the results table
static const int NUM_COLS = 11;

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

// ===== THE FOUR DIFFERENTIATION METHODS =====

// forward difference: (f(x+h) - f(x)) / h
double Differentiation::forwardDifference(MathFunction f, double x, double h) {
    return (f(x + h) - f(x)) / h;
}

// backward difference: (f(x) - f(x-h)) / h
double Differentiation::backwardDifference(MathFunction f, double x, double h) {
    return (f(x) - f(x - h)) / h;
}

// central difference: (f(x+h) - f(x-h)) / (2h)
double Differentiation::centralDifference(MathFunction f, double x, double h) {
    return (f(x + h) - f(x - h)) / (2.0 * h);
}

// richardson extrapolation: (4*D(h/2) - D(h)) / 3
// where D is central difference
double Differentiation::richardsonExtrapolation(MathFunction f, double x, double h) {
    double D_h = centralDifference(f, x, h);
    double D_h2 = centralDifference(f, x, h / 2.0);
    return (4.0 * D_h2 - D_h) / 3.0;
}

// ===== COMPUTE ALL =====

// compute all methods for all registered functions at all h values
// populates the inherited Matrix data[][] with the results table
//
// Matrix layout: (numFunctions * numH) rows × 11 columns
// Col 0: func_index  Col 1: h       Col 2: exact
// Col 3: forward     Col 4: fwd_err Col 5: backward   Col 6: bwd_err
// Col 7: central     Col 8: cen_err Col 9: richardson  Col 10: rich_err
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

            double fwd = forwardDifference(f, x0, h);
            double bwd = backwardDifference(f, x0, h);
            double cen = centralDifference(f, x0, h);
            double rich = richardsonExtrapolation(f, x0, h);

            // store in Matrix data[][]
            data[row][0]  = (double)fi;              // func index
            data[row][1]  = h;                       // step size
            data[row][2]  = exact;                   // exact derivative
            data[row][3]  = fwd;                     // forward approx
            data[row][4]  = fabs(exact - fwd);       // forward error
            data[row][5]  = bwd;                     // backward approx
            data[row][6]  = fabs(exact - bwd);       // backward error
            data[row][7]  = cen;                     // central approx
            data[row][8]  = fabs(exact - cen);       // central error
            data[row][9]  = rich;                    // richardson approx
            data[row][10] = fabs(exact - rich);      // richardson error

            // store function name for this row
            functionNames[row] = functions[fi].name;

            row++;
        }
    }

    cout << "Computed derivatives for " << numFunctions << " functions × "
         << numH << " step sizes (" << totalRows << " rows)" << endl;
}

// ===== DISPLAY =====

// display results as a nicely formatted table with function names
void Differentiation::display() const {
    if (rows == 0 || cols == 0) {
        cout << "No results computed yet... call computeAll() first" << endl;
        return;
    }

    // column headers
    cout << left << setw(18) << "Function"
         << right
         << setw(14) << "h"
         << setw(14) << "Exact"
         << setw(14) << "Forward"
         << setw(14) << "Fwd_Err"
         << setw(14) << "Backward"
         << setw(14) << "Bwd_Err"
         << setw(14) << "Central"
         << setw(14) << "Cen_Err"
         << setw(14) << "Richardson"
         << setw(14) << "Rich_Err"
         << endl;

    // separator line
    cout << string(18 + 14 * 10, '-') << endl;

    cout << scientific << setprecision(6);

    for (int i = 0; i < rows; i++) {
        // function name (use stored name, not the index)
        string fname = (functionNames != nullptr) ? functionNames[i] : "?";
        cout << left << setw(18) << fname;

        // h value
        cout << right << setw(14) << data[i][1];

        // exact through richardson error (cols 2-10)
        for (int j = 2; j < NUM_COLS; j++) {
            cout << setw(14) << data[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

// ===== SAVE RESULTS =====

// save results to file in scientific notation (matching Python output format)
void Differentiation::saveResults(string filename) const {
    if (rows == 0 || cols == 0)
        throw MatrixException("no results to save... call computeAll() first");

    ofstream fout(filename);
    if (!fout)
        throw MatrixException("can't write to file '" + filename + "'");

    // write header
    fout << left << setw(18) << "Function"
         << right
         << setw(14) << "h"
         << setw(14) << "Exact"
         << setw(14) << "Forward_approx"
         << setw(14) << "Forward_error"
         << setw(14) << "Backward_approx"
         << setw(14) << "Backward_error"
         << setw(14) << "Central_approx"
         << setw(14) << "Central_error"
         << setw(14) << "Richardson_approx"
         << setw(14) << "Richardson_error"
         << "\n";

    fout << scientific << setprecision(8);

    for (int i = 0; i < rows; i++) {
        string fname = (functionNames != nullptr) ? functionNames[i] : "?";
        fout << left << setw(18) << fname;

        fout << right << setw(14) << data[i][1];

        for (int j = 2; j < NUM_COLS; j++) {
            fout << setw(16) << data[i][j];
        }
        fout << "\n";
    }

    fout.close();
    cout << "Results saved to " << filename << endl;
}
