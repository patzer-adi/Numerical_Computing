#include "include/RootHunter.hpp"
#include "utils/InputUtils.hpp"
#include "utils/DisplayUtils.hpp"
#include <iostream>

int main() {
    double start = readDouble("Enter starting value: ");
    double step  = readDouble("Enter step size: ");
    double tol   = readDouble("Enter tolerance: ");

    RootHunter solver(tol);

    if (solver.findInterval(start, step, 50)) {
        solver.solve();
        printResult(solver.getRoot(), solver.getIterations());
    } else {
        std::cout << "Program ended: no root in given range.\n";
    }

    return 0;
}
