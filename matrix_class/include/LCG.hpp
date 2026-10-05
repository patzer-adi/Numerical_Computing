#ifndef LCG_HPP
#define LCG_HPP

#include "RandomNumberGenerator.hpp"

// LCG — Linear Congruential Generator
//
//     X_{n+1} = (a * X_n + c) mod m
//     U_n = X_n / m
//
// Default parameters: glibc constants
//     a = 1103515245
//     c = 12345
//     m = 2^31 = 2147483648
//
// Constraints:
//     m > 0
//     0 <= a < m
//     0 <= c < m
//     0 <= seed < m
//
// Python equivalent: pynumerics.rng.lcg.LCG

class LCG : public RandomNumberGenerator {
private:
    unsigned long a;       // multiplier
    unsigned long c;       // increment
    unsigned long m;       // modulus
    unsigned long state;   // current X_n

public:
    // constructors / destructor
    LCG();
    LCG(unsigned long seed, unsigned long a = 1103515245,
        unsigned long c = 12345, unsigned long m = 2147483648UL);
    ~LCG();

    // core recurrence: X_{n+1} = (a * X_n + c) mod m
    unsigned long nextInt() override;
    string getMethodName() const override;
    void reset() override;

    // getters
    unsigned long getState() const;
    unsigned long getA() const;
    unsigned long getC() const;
    unsigned long getM() const;
};

#endif
