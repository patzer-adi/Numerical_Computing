#include "utils/DisplayUtils.hpp"
#include "utils/InputUtils.hpp"
#include "../include/RootHunter.hpp"
#include "../include/FixedPoint.hpp"
#include "../include/NewtonRaphson.hpp"
#include "../include/Bisection.hpp"
#include<iostream>
using namespace std;

int main() {

    double tol = getTolerance();
    string string_method;
    // Bisection b(tol);
    // b.input();
    // b.solve();
    // displayResult("Bisection", b.getRoot(), b.getIterations());
    //
    // NewtonRaphson n(tol);
    // n.input();
    // n.solve();
    // displayResult("Newton Raphson", n.getRoot(), n.getIterations());
    //
    // FixedPoint f(tol);
    // f.input();
    // f.solve();
    // displayResult("Fixed Point", f.getRoot(), f.getIterations());
    //

    RootHunter* method = nullptr;
    int choice;

    cout << "Select method:\n";
    cout << "1. Bisection\n";
    cout << "2. Newton Raphson\n";
    cout << "3. Fixed Point iteration\n";
    cin >> choice;

    if(choice == 1) {
        method = new Bisection(tol);
        string_method = "Bisection";
    }
    else if(choice == 2) {
        method = new NewtonRaphson(tol);
        string_method = "Newton Raphson";
    }
    else if(choice == 3) {
        method = new FixedPoint(tol);
        string_method = "Fixed Point";
    }
    else {
        cout << "Invalid choice\n";
        return 0;
    }

    method->input();
    method->solve();

    displayResult(string_method,method->getRoot(),method->getIterations());

    delete method;
    return 0;
}
