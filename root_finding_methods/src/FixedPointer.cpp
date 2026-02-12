#include "../include/RootHunter.hpp"
#include "../include/FixedPoint.hpp"
#include "../include/NewtonRaphson.hpp"
#include "../include/Bisection.hpp"

#include "../utils/DisplayUtils.hpp"
#include <iostream>
using namespace std;



FixedPoint::FixedPoint(double tol) : RootHunter(tol) {}


void FixedPoint::input() {
    cout << "\nFixed Point Input\n";
    cout << "Enter initial guess: ";
    cin >> guess;
}

void FixedPoint::solve() {
    double previous = guess;
    double current = g(previous);

    while (fabs(current - previous) > tolerance) {
        previous = current;
        current = g(previous);
        iterations++;
    }

    root = current;
}
