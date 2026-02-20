#include "../include/RootHunter.hpp"
#include "../include/FixedPoint.hpp"
#include "../include/NewtonRaphson.hpp"
#include "../include/Bisection.hpp"

#include "../utils/DisplayUtils.hpp"
#include <iostream>
using namespace std;

double RootHunter::f(double x) {
    //return x*x*x - x - 2;
    return x*x*x - 2*x- - 5;
}

double FixedPoint::g(double x) {
    //return cbrt(x + 2); // rearranged from x^3 - x - 2 = 0
    return cbrt(2*x + 5);
}

double NewtonRaphson::df(double x) {
    //return 3*x*x - 1;
    return 3*x*x - 2;
}


RootHunter::RootHunter(double tol) {
    tolerance = tol;
    iterations = 0;
    root = 0;
}

double RootHunter::getRoot() const {
    return root;
}

int RootHunter::getIterations() const {
    return iterations;
}




