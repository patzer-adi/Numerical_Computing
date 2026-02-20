#include "DisplayUtils.hpp"
#include <iostream>
using namespace std;

void displayResult(std::string method, double root, int iterations) {
    cout << "\n--- " << method << " Result ---\n";
    cout << "Root: " << root << "\n";
    cout << "Iterations: " << iterations << "\n";
}
