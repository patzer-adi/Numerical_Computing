#include "../include/RandomNumberGenerator.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>

// default constructor
RandomNumberGenerator::RandomNumberGenerator()
    : initialSeed(0), seed(0), modulus(1) {}

// parameterized constructor
RandomNumberGenerator::RandomNumberGenerator(unsigned long seed, unsigned long modulus)
    : initialSeed(seed), seed(seed), modulus(modulus) {}

// destructor
RandomNumberGenerator::~RandomNumberGenerator() {}

// normalized uniform value in [0, 1)
double RandomNumberGenerator::nextUniform() {
    return (double)nextInt() / (double)modulus;
}

// generate N uniform values
vector<double> RandomNumberGenerator::generate(int n) {
    if (n < 0)
        throw invalid_argument("n must be non-negative");

    vector<double> samples;
    samples.reserve(n);
    for (int i = 0; i < n; i++)
        samples.push_back(nextUniform());
    return samples;
}

// generate N raw integer states
vector<unsigned long> RandomNumberGenerator::generateInts(int n) {
    if (n < 0)
        throw invalid_argument("n must be non-negative");

    vector<unsigned long> raw;
    raw.reserve(n);
    for (int i = 0; i < n; i++)
        raw.push_back(nextInt());
    return raw;
}

// reset to initial seed
void RandomNumberGenerator::reset() {
    seed = initialSeed;
}

// display formatted table
void RandomNumberGenerator::display(const vector<double> &samples) const {
    cout << endl << "=== " << getMethodName() << " ===" << endl;
    cout << "Seed: " << initialSeed << endl;
    cout << endl;

    cout << setw(8) << right << "n"
         << setw(16) << right << "U_n" << endl;
    cout << string(24, '-') << endl;

    for (int i = 0; i < (int)samples.size(); i++) {
        cout << setw(8) << right << (i + 1)
             << setw(16) << right << fixed << setprecision(6)
             << samples[i] << endl;
    }
    cout << endl;
}

// save to file
void RandomNumberGenerator::saveResults(const string &filename,
                                        const vector<double> &samples) const {
    ofstream fout(filename.c_str());
    if (!fout.is_open())
        throw runtime_error("could not open file: " + filename);

    fout << "# " << getMethodName() << endl;
    fout << "# Seed: " << initialSeed << endl;
    fout << setw(8) << right << "n"
         << setw(16) << right << "U_n" << endl;

    for (int i = 0; i < (int)samples.size(); i++) {
        fout << setw(8) << right << (i + 1)
             << setw(16) << right << fixed << setprecision(10)
             << samples[i] << endl;
    }

    fout.close();
    cout << "Results saved to " << filename << endl;
}

// getters
unsigned long RandomNumberGenerator::getSeed() const { return initialSeed; }
unsigned long RandomNumberGenerator::getModulus() const { return modulus; }
