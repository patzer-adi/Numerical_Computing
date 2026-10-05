#ifndef RANDOMNUMBERGENERATOR_HPP
#define RANDOMNUMBERGENERATOR_HPP

#include <string>
#include <vector>
using namespace std;

// RandomNumberGenerator — abstract base class for pseudo-random number generators
//
// Independent of Matrix — the RNG is a stateful generator, not a data container.
// Follows a similar pattern to the Interpolation base class (independent hierarchy).
//
// Hierarchy:
//     RandomNumberGenerator (abstract base)
//         └── LCG
//         └── (future: MersenneTwister, etc.)
//
// Future Monte Carlo classes will use RNG via composition, not inheritance:
//     MonteCarlo uses→ RandomNumberGenerator
//

class RandomNumberGenerator {
protected:
    unsigned long initialSeed;
    unsigned long seed;
    unsigned long modulus;

public:
    // constructors / destructor
    RandomNumberGenerator();
    RandomNumberGenerator(unsigned long seed, unsigned long modulus);
    virtual ~RandomNumberGenerator();

    // --- pure virtual: each derived class provides its own recurrence ---
    virtual unsigned long nextInt() = 0;

    // --- pure virtual: each derived class returns its method name ---
    virtual string getMethodName() const = 0;

    // normalized uniform value in [0, 1)
    double nextUniform();

    // generate N uniform values
    vector<double> generate(int n);

    // generate N raw integer states
    vector<unsigned long> generateInts(int n);

    // reset to initial seed
    virtual void reset();

    // display formatted table
    void display(const vector<double> &samples) const;

    // save to file
    void saveResults(const string &filename, const vector<double> &samples) const;

    // getters
    unsigned long getSeed() const;
    unsigned long getModulus() const;
};

#endif
