#include "InputUtils.hpp"
#include <iostream>

double readDouble(const char* val) {
    double value;
    std::cout << val;
    std::cin >> value;
    return value;
}
