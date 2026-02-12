#ifndef FIXEDPOINT_HPP
#define FIXEDPOINT_HPP
#include <iostream>
#include <cmath>
#include "RootHunter.hpp"

class FixedPoint : public RootHunter {
private:
    double guess;

    double g(double x);

public:
    FixedPoint(double tol);
    void input() override;
    void solve() override;
};

#endif
