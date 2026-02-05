#include "RootHunter.hpp"
#include <iostream>
#include <cmath>
using namespace std;
RootHunter::RootHunter(double tol)
: left(0), right(0), tolerance(tol), root(0), iterations(0) {}

double RootHunter::f(double x) {
    // Example function
    return 4*x*x*x - 3*x;
}

bool RootHunter::findInterval(double start, double step, int maxSteps) {
    left = start;
    right = start + step;

    cout << "\nSearching for a valid interval...\n";

    for (int i = 0; i < maxSteps; i++) {
        cout << "Checking interval ["
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

    cout << "Failed to find a valid interval.\n";
    return false;
}

void RootHunter::solve() {
    cout << "\nStarting bisection method...\n";

    double previous = left;
    double current = (left + right) / 2.0;

    while (abs(current - previous) >= tolerance) {
        cout << "Iteration " << iterations << ":\n";
        cout << "  Previous = " << previous << "\n";
        cout << "  Current  = " << current
        << ", f(Current) = " << f(current) << "\n";

        if (f(current) == 0.0) {
            cout << "Exact root found.\n";
            break;
        }

        if (f(left) * f(current) < 0) {
            cout << "  Root is in left subinterval.\n";
            right = current;
        } else {
            cout << "  Root is in right subinterval.\n";
            left = current;
        }

        previous = current;
        current = (left + right) / 2.0;

        iterations++;
    }

    root = current;
}


double RootHunter::getRoot()  {
    return root;
}

int RootHunter::getIterations()  {
    return iterations;
}

