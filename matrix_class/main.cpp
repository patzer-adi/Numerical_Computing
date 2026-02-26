
so i am writing matrix class file:
these are all the concoepts from c++ oops code: i have to implement: inheritence, virtual functiion and virtual class (only the place where it is necessary and only if it is necessary), operator overloading, polymorphism,  throw and catch statements i.e exception handling,  encaplusaion, abstraction, pragams and header file protection those pragma commands....

i have to have one hpp file where all the matrix header fundtions are defined in includes and then i have this  directory called as src where we have to have cpp code... cpp code like for each derived class it can have a different cpp file or for a fucntion that is too specific we can have different cpp file for it... one single main file and then we have a a utils file where we store the display and input utils files and then we have a the hpp files and cpp files required...

so for now i what i was thinking is that have a Matrix class and then underneat have MatrixException class which is a derived class maybe.... if this is wrong tell me directly.... and give me the proper class file structure

so the functionalities we have to implement in the class file for now are:
1) matrix addtion, its basic add() function and its operator overlaoding
2) matrix subtractipn, its basic mul() function and its its operator overloading
3) we have to have this GaussianEliminatiion implementation, with pivoting and without pivoting...
4) we have to give matrix using comand line interpreter and then we have to give matrix usiing file... so we have to write both but using command like we have to give the file using command line, like a it should open the file....  and the files are like the ones i have attached... so if a files lke i have atttached are given... r and l that is left is the matrix that is nxn so even if the file input is like '56l.txt' or '56right' we have to understand it specifically accordingly... like parse it accordinly and then agument the matrix and then process it
and even if we are given the matrix that is agumented already we have to understand it accordingly
5) we have to find the determinant of the matrix one with recursion and one without... the one without is where i will we will store all the determinats from 1x1 or 2x2, 3x3 till we have nxn... like everyminor and we can get the values of the detminant easily for any minor as well
6) create a template for LU decomposition, i have to implement dolittle method, cholesky method and then i have to implement cruts method
7) also create a template for matrix multiplication...

first give me the template to study the classfile i.e our header file only after i approve proceed further#pragma once
#include <vector>
#include <string>
#include <iostream>
// Forward declaration
class MatrixException;
// ===============================
// Base Class
// ===============================
class Matrix {
protected:
    size_t rows;
    size_t cols;
    std::vector<std::vector<double>> data;
    void checkSquare() const;
    void checkSameSize(const Matrix& other) const;
public:
    // Constructors
    Matrix();
    Matrix(size_t r, size_t c);
    Matrix(const std::vector<std::vector<double>>& values);
    virtual ~Matrix() = default;
    // Access
    size_t getRows() const;
    size_t getCols() const;
    double& operator()(size_t i, size_t j);
    double operator()(size_t i, size_t j) const;
    // Basic operations
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    // Determinant
    double determinantRecursive() const;
    double determinantGaussian() const;
    // File input
    static Matrix fromFile(const std::string& filename);
    // Display
    virtual void print(std::ostream& os = std::cout) const;
};

#pragma once
#include "Matrix.hpp"
// ===============================
// Derived Class
// ===============================
class GaussianElimination : public Matrix {
public:
    GaussianElimination(const Matrix& m);
    // Core functionality
    Matrix eliminateWithoutPivot();
    Matrix eliminateWithPivot();
    // Solve Ax = b if augmented
    std::vector<double> backSubstitution() const;
    // Virtual override (polymorphism)
    void print(std::ostream& os = std::cout) const override;
};
#pragma once
#include "Matrix.hpp"
#include <utility>
// ===============================
// Abstract LU Base Class
// ===============================
class LUDecomposition : public Matrix {
public:
    LUDecomposition(const Matrix& m);
    virtual ~LUDecomposition() = default;
    // Pure virtual function
    virtual std::pair<Matrix, Matrix> decompose() = 0;
};
class Doolittle : public LUDecomposition {
public:
    Doolittle(const Matrix& m);
    std::pair<Matrix, Matrix> decompose() override;
};
class Crout : public LUDecomposition {
public:
    Crout(const Matrix& m);
    std::pair<Matrix, Matrix> decompose() override;
};
class Cholesky : public LUDecomposition {
public:
    Cholesky(const Matrix& m);
    std::pair<Matrix, Matrix> decompose() override;
};
#pragma once
#include <exception>
#include <string>
class MatrixException : public std::exception {
    std::string message;
public:
    MatrixException(const std::string& msg);
    const char* what() const noexcept override;
};
Q
: How should comments feel in the code?
A: Minimal: almost no comments

Q: For things like determinant or Gaussian elimination, should the logic be written out manually (loops, manual swaps) or can it use some STL like std::swap?
A: Fully manual, no STL tricks

Q: Should the student have made any small beginner mistakes that are then corrected (like starting with a flat array then switching to 2D vector), or should it just feel beginner-written but still clean and correct?
A: Clean but beginner-style
