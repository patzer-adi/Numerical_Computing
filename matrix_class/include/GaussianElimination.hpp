#pragma once

#include "Matrix.hpp"
#include <vector>

class GaussianElimination : public Matrix {
public:
    GaussianElimination(const Matrix& m);

    Matrix eliminateWithoutPivot();
    Matrix eliminateWithPivot();

    std::vector<double> backSubstitution() const;

    void print(std::ostream& os = std::cout) const override;
};
