#pragma once

#include "Matrix.hpp"
#include <utility>

class LUDecomposition : public Matrix {
public:
    LUDecomposition(const Matrix& m);
    virtual ~LUDecomposition();

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
