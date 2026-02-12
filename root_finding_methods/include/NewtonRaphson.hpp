#ifndef NEWTONRAPHSON_HPP
#define NEWTONRAPHSON_HPP
#include "RootHunter.hpp"

class NewtonRaphson : public RootHunter {
private:
    double guess;
    double df(double x);

public:
    NewtonRaphson(double tol);
    void input() override;
    void solve() override;
};
#endif
