#pragma once

#include <iostream>
#include <vector>
#include "../include/Matrix.hpp"

namespace Display {

    void printMatrix(const Matrix& m, std::ostream& os = std::cout);

    void printVector(const std::vector<double>& v,
                     std::ostream& os = std::cout);

    void printLU(const Matrix& L, const Matrix& U,
                 std::ostream& os = std::cout);

    void printLine(std::ostream& os = std::cout);
}
