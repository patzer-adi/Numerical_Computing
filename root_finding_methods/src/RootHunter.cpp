#include "RootHunter.hpp"
#include <iostream>
#include <cmath>

RootHunter::RootHunter(double tol)
: left(0), right(0), tolerance(tol), root(0), iterations(0) {}

double RootHunter::f(double x) {
    // Example function
    return 4*x*x*x - 3*x;
}

bool RootHunter::findInterval(double start, double step, int maxSteps) {
    left = start;
    right = start + step;

    std::cout << "\nSearching for a valid interval...\n";

    for (int i = 0; i < maxSteps; i++) {
        std::cout << "Checking interval ["
        << left << ", " << right << "] "
        << "f(a) = " << f(left)
        << ", f(b) = " << f(right) << "\n";

        if (f(left) * f(right) < 0) {
            std::cout << "Valid interval found.\n";
            return true;
        }

        left = right;
        right += step;
    }

    std::cout << "Failed to find a valid interval.\n";
    return false;
}

void RootHunter::solve() {
    std::cout << "\nStarting bisection method...\n";

    double mid;

    while ((right - left) / 2.0 > tolerance) {
        mid = (left + right) / 2.0;

        std::cout << "Iteration " << iterations << ":\n";
        std::cout << "  Left = " << left
        << ", Right = " << right << "\n";
        std::cout << "  Mid  = " << mid
        << ", f(Mid) = " << f(mid) << "\n";

        if (f(mid) == 0.0) {
            std::cout << "Exact root found.\n";
            break;
        }

        if (f(left) * f(mid) < 0) {
            std::cout << "  Root is in left subinterval.\n";
            right = mid;
        } else {
            std::cout << "  Root is in right subinterval.\n";
            left = mid;
        }

        iterations++;
    }

    root = (left + right) / 2.0;
}

double RootHunter::getRoot() const {
    return root;
}

int RootHunter::getIterations() const {
    return iterations;
}

