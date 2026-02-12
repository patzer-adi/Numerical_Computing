#include "../include/RootHunter.hpp"
#include "../include/FixedPoint.hpp"
#include "../include/NewtonRaphson.hpp"
#include "../include/Bisection.hpp"

#include "../utils/DisplayUtils.hpp"
#include <iostream>
using namespace std;

Bisection::Bisection(double tol) : RootHunter(tol) {}


bool Bisection::findInterval(double start, double step, int maxSteps) {
    left = start;
    right = start + step;
    bool found = false;
    for (int i = 0; i < maxSteps; i++) {
        // cout << "Checking interval ["
        // << left << ", " << right << "] "
        // << "f(a) = " << f(left)
        // << ", f(b) = " << f(right) << "\n";

        if (f(left) * f(right) < 0) {
            cout << "Valid interval found.\n";
            return true;
        }

        left = right;
        right += step;
    }
    if(!found)
    {
        left = -start;
        right = - (start + step);
        for (int i = 0; i < maxSteps; i++) {
            // cout << "Checking interval ["
            // << left << ", " << right << "] "
            // << "f(a) = " << f(left)
            // << ", f(b) = " << f(right) << "\n";

            if (f(left) * f(right) < 0) {
                cout << "Valid interval found.\n";
                return true;
            }

            left = right;
            right += step;
        }
    }

    if(!found){

        cout << "Failed to find a valid interval.\n";
        //return false;
    }
    return false;
}



void Bisection::input() {
    double start, step;
    int maxSteps;

    cout << "\nBisection Method Input\n";
    cout << "Start value: ";
    cin >> start;
    cout << "Step size: ";
    cin >> step;
    cout << "Max steps: ";
    cin >> maxSteps;

    findInterval(start, step, maxSteps);
}

void Bisection::solve() {
    double previous = left;
    double current = (left + right) / 2.0;

    while (fabs(current - previous) > tolerance) {
        previous = current;

        if (f(left) * f(current) < 0)
            right = current;
        else
            left = current;

        current = (left + right) / 2.0;
        iterations++;
    }

    root = current;
}
