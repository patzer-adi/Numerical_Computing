#ifndef ROOTHUNTER_HPP
#define ROOTHUNTER_HPP

#include <iostream>
#include <cmath>

class RootHunter {
protected:
    double left, right;
    double root;
    double tolerance;
    int iterations;

    double f(double x);

   // double df(double x);

public:
    RootHunter(double tol);
    virtual ~RootHunter() {}

    virtual void input() = 0;
    virtual void solve() = 0;

    double getRoot() const;
    int getIterations() const;
};

#endif




