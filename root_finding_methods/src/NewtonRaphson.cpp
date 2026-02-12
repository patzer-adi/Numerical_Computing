#include "../include/RootHunter.hpp"
#include "../include/FixedPoint.hpp"
#include "../include/NewtonRaphson.hpp"
#include "../include/Bisection.hpp"

#include "../utils/DisplayUtils.hpp"
#include <iostream>
using namespace std;

#include <cmath>

NewtonRaphson::NewtonRaphson(double tol) : RootHunter(tol) {}

void NewtonRaphson::input() {
    std::cout << "\nNewton Raphson Input\n";
    std::cout << "Enter initial guess: ";
    std::cin >> guess;
}

void NewtonRaphson::solve() {
    double previous = guess;
    double current = previous - f(previous) / df(previous);

    while (fabs(current - previous) > tolerance) {
        previous = current;
        current = previous - f(previous) / df(previous);
        iterations++;
    }

    root = current;
}
