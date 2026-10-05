#include "../include/LCG.hpp"
#include <stdexcept>
#include <sstream>

// default constructor — glibc constants
LCG::LCG()
    : RandomNumberGenerator(42, 2147483648UL),
      a(1103515245), c(12345), m(2147483648UL), state(42) {}

// parameterized constructor
LCG::LCG(unsigned long seed, unsigned long a, unsigned long c, unsigned long m)
    : RandomNumberGenerator(seed, m), a(a), c(c), m(m), state(seed) {

    // validate parameters
    if (m == 0) {
        throw invalid_argument("modulus m must be positive");
    }
    if (a >= m) {
        ostringstream msg;
        msg << "multiplier a must satisfy 0 <= a < m, got a=" << a << ", m=" << m;
        throw invalid_argument(msg.str());
    }
    if (c >= m) {
        ostringstream msg;
        msg << "increment c must satisfy 0 <= c < m, got c=" << c << ", m=" << m;
        throw invalid_argument(msg.str());
    }
    if (seed >= m) {
        ostringstream msg;
        msg << "seed must satisfy 0 <= seed < m, got seed=" << seed << ", m=" << m;
        throw invalid_argument(msg.str());
    }
}

// destructor
LCG::~LCG() {}

// core recurrence: X_{n+1} = (a * X_n + c) mod m
unsigned long LCG::nextInt() {
    state = (a * state + c) % m;
    return state;
}

string LCG::getMethodName() const {
    return "Linear Congruential Generator (LCG)";
}

void LCG::reset() {
    RandomNumberGenerator::reset();
    state = initialSeed;
}

// getters
unsigned long LCG::getState() const { return state; }
unsigned long LCG::getA() const { return a; }
unsigned long LCG::getC() const { return c; }
unsigned long LCG::getM() const { return m; }
