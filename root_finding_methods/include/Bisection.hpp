#ifndef BISECTION_HPP
#define BISECTION_HPP

#include "RootHunter.hpp"

class Bisection : public RootHunter {
public:
    Bisection(double tol);
    bool findInterval(double start, double step, int maxSteps);
    void input() override;
    void solve() override;
};
#endif
