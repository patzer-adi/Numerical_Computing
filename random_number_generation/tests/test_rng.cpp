#include "../include/LCG.hpp"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cmath>
#include <vector>
using namespace std;

int main() {
    int passed = 0;
    int failed = 0;

    // Test 1: known sequence with small parameters
    {
        LCG lcg(0, 5, 3, 16);
        bool ok = true;
        // X_1 = (5*0 + 3) % 16 = 3
        if (lcg.nextInt() != 3) ok = false;
        // X_2 = (5*3 + 3) % 16 = 2
        if (lcg.nextInt() != 2) ok = false;
        // X_3 = (5*2 + 3) % 16 = 13
        if (lcg.nextInt() != 13) ok = false;
        // X_4 = (5*13 + 3) % 16 = 4
        if (lcg.nextInt() != 4) ok = false;
        if (ok) { cout << "PASS  test_known_sequence_small" << endl; passed++; }
        else    { cout << "FAIL  test_known_sequence_small" << endl; failed++; }
    }

    // Test 2: glibc defaults — first value
    {
        LCG lcg(0);
        unsigned long x = lcg.nextInt();
        // X_1 = (1103515245 * 0 + 12345) % 2^31 = 12345
        if (x == 12345) { cout << "PASS  test_glibc_first_value" << endl; passed++; }
        else            { cout << "FAIL  test_glibc_first_value (got " << x << ")" << endl; failed++; }
    }

    // Test 3: reproducibility
    {
        LCG lcg1(42);
        LCG lcg2(42);
        vector<double> s1 = lcg1.generate(100);
        vector<double> s2 = lcg2.generate(100);
        if (s1 == s2) { cout << "PASS  test_reproducibility" << endl; passed++; }
        else          { cout << "FAIL  test_reproducibility" << endl; failed++; }
    }

    // Test 4: different seeds
    {
        LCG lcg1(1);
        LCG lcg2(2);
        vector<double> s1 = lcg1.generate(100);
        vector<double> s2 = lcg2.generate(100);
        if (s1 != s2) { cout << "PASS  test_different_seeds" << endl; passed++; }
        else          { cout << "FAIL  test_different_seeds" << endl; failed++; }
    }

    // Test 5: reset
    {
        LCG lcg(42);
        vector<double> s1 = lcg.generate(50);
        lcg.reset();
        vector<double> s2 = lcg.generate(50);
        if (s1 == s2) { cout << "PASS  test_reset" << endl; passed++; }
        else          { cout << "FAIL  test_reset" << endl; failed++; }
    }

    // Test 6: generate count
    {
        LCG lcg(0);
        vector<double> s = lcg.generate(1000);
        if ((int)s.size() == 1000) { cout << "PASS  test_generate_count" << endl; passed++; }
        else                       { cout << "FAIL  test_generate_count" << endl; failed++; }
    }

    // Test 7: uniform range [0, 1)
    {
        LCG lcg(42);
        vector<double> s = lcg.generate(10000);
        bool ok = true;
        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i] < 0.0 || s[i] >= 1.0) { ok = false; break; }
        }
        if (ok) { cout << "PASS  test_uniform_range" << endl; passed++; }
        else    { cout << "FAIL  test_uniform_range" << endl; failed++; }
    }

    // Test 8: normalization U = X / m
    {
        LCG lcg(7, 13, 5, 100);
        unsigned long x = lcg.nextInt();
        lcg.reset();
        double u = lcg.nextUniform();
        double expected = (double)x / 100.0;
        if (fabs(u - expected) < 1e-12) { cout << "PASS  test_normalization" << endl; passed++; }
        else                            { cout << "FAIL  test_normalization" << endl; failed++; }
    }

    // Test 9: small cycle (a=1, c=1, m=4)
    {
        LCG lcg(0, 1, 1, 4);
        vector<unsigned long> vals = lcg.generateInts(8);
        bool ok = true;
        unsigned long expected[] = {1, 2, 3, 0, 1, 2, 3, 0};
        for (int i = 0; i < 8; i++) {
            if (vals[i] != expected[i]) { ok = false; break; }
        }
        if (ok) { cout << "PASS  test_small_cycle" << endl; passed++; }
        else    { cout << "FAIL  test_small_cycle" << endl; failed++; }
    }

    // Test 10: method name
    {
        LCG lcg(42);
        string name = lcg.getMethodName();
        if (name.find("LCG") != string::npos) { cout << "PASS  test_method_name" << endl; passed++; }
        else                                   { cout << "FAIL  test_method_name" << endl; failed++; }
    }

    // Test 11: mean ≈ 0.5 (sanity)
    {
        LCG lcg(42);
        vector<double> s = lcg.generate(10000);
        double sum = 0;
        for (int i = 0; i < (int)s.size(); i++) sum += s[i];
        double mean = sum / s.size();
        if (fabs(mean - 0.5) < 0.02) { cout << "PASS  test_mean_sanity" << endl; passed++; }
        else                          { cout << "FAIL  test_mean_sanity (got " << mean << ")" << endl; failed++; }
    }

    // Test 12: C++/Python cross-validation — first 10 values with seed=42, glibc defaults
    // These must match Python's LCG(seed=42).generate(10) exactly
    {
        LCG lcg(42);
        vector<double> s = lcg.generate(10);
        double py_ref[10] = {
            0.5823075897, 0.5198187493, 0.4659764250, 0.7770372583, 0.4228650290,
            0.0333723295, 0.4173891307, 0.8087285170, 0.6123396843, 0.7149040475
        };
        bool ok = true;
        for (int i = 0; i < 10; i++) {
            if (fabs(s[i] - py_ref[i]) > 1e-8) { ok = false; break; }
        }
        if (ok) { cout << "PASS  test_cross_validation_exact_python" << endl; passed++; }
        else    { cout << "FAIL  test_cross_validation_exact_python" << endl; failed++; }
    }

    // Test 13: getters
    {
        LCG lcg(10, 7, 3, 32);
        bool ok = (lcg.getSeed() == 10 && lcg.getA() == 7 &&
                   lcg.getC() == 3 && lcg.getM() == 32 &&
                   lcg.getState() == 10 && lcg.getModulus() == 32);
        if (ok) { cout << "PASS  test_getters" << endl; passed++; }
        else    { cout << "FAIL  test_getters" << endl; failed++; }
    }

    // Test 14: validation m == 0 throws invalid_argument
    {
        bool caught = false;
        try {
            LCG lcg(0, 1, 0, 0);
        } catch (const invalid_argument &) {
            caught = true;
        }
        if (caught) { cout << "PASS  test_validation_m_zero" << endl; passed++; }
        else        { cout << "FAIL  test_validation_m_zero" << endl; failed++; }
    }

    // Test 15: validation a >= m throws invalid_argument
    {
        bool caught = false;
        try {
            LCG lcg(0, 32, 1, 32);
        } catch (const invalid_argument &) {
            caught = true;
        }
        if (caught) { cout << "PASS  test_validation_a_ge_m" << endl; passed++; }
        else        { cout << "FAIL  test_validation_a_ge_m" << endl; failed++; }
    }

    // Test 16: validation c >= m throws invalid_argument
    {
        bool caught = false;
        try {
            LCG lcg(0, 5, 32, 32);
        } catch (const invalid_argument &) {
            caught = true;
        }
        if (caught) { cout << "PASS  test_validation_c_ge_m" << endl; passed++; }
        else        { cout << "FAIL  test_validation_c_ge_m" << endl; failed++; }
    }

    // Test 17: validation seed >= m throws invalid_argument
    {
        bool caught = false;
        try {
            LCG lcg(32, 5, 1, 32);
        } catch (const invalid_argument &) {
            caught = true;
        }
        if (caught) { cout << "PASS  test_validation_seed_ge_m" << endl; passed++; }
        else        { cout << "FAIL  test_validation_seed_ge_m" << endl; failed++; }
    }

    // Test 18: generate negative count throws invalid_argument
    {
        bool caught = false;
        try {
            LCG lcg(42);
            lcg.generate(-5);
        } catch (const invalid_argument &) {
            caught = true;
        }
        if (caught) { cout << "PASS  test_validation_negative_n" << endl; passed++; }
        else        { cout << "FAIL  test_validation_negative_n" << endl; failed++; }
    }

    // Summary
    cout << "\n============================" << endl;
    cout << passed << " passed, " << failed << " failed" << endl;
    if (failed == 0)
        cout << "ALL TESTS PASSED" << endl;
    cout << "============================" << endl;

    return failed;
}
