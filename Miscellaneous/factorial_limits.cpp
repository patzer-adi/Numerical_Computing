#include <iostream>
#include <iomanip>
#include <cmath>
#include <cfloat>
#include <limits>

using namespace std;

// Function to compute factorial for int
void factorial_int() {
    cout << "\n========== INT FACTORIAL ==========\n";
    cout << "Size: " << sizeof(int) << " bytes\n";
    cout << "Max value: " << INT_MAX << "\n";
    
    int result = 1;
    for (int n = 1; n <= 100; n++) {
        result *= n;
        
        // Check for overflow
        if (result < 0 || (n > 1 && result / n != (result == 0 ? 0 : result / n))) {
            cout << "Overflow at n = " << n << "\n";
            cout << "Maximum n for int: " << (n - 1) << "\n";
            cout << (n-1) << "! = " << result/n << "\n";
            break;
        }
        
        if (n <= 20 || n % 5 == 0) {
            cout << n << "! = " << result << "\n";
        }
    }
}

// Function to compute factorial for unsigned int
void factorial_unsigned_int() {
    cout << "\n========== UNSIGNED INT FACTORIAL ==========\n";
    cout << "Size: " << sizeof(unsigned int) << " bytes\n";
    cout << "Max value: " << UINT_MAX << "\n";
    
    unsigned int result = 1;
    for (unsigned int n = 1; n <= 100; n++) {
        // Check for overflow before multiplication
        if (result > UINT_MAX / n) {
            cout << "Overflow would occur at n = " << n << "\n";
            cout << "Maximum n for unsigned int: " << (n - 1) << "\n";
            cout << (n-1) << "! = " << result << "\n";
            break;
        }
        
        result *= n;
        
        if (n <= 20 || n % 5 == 0) {
            cout << n << "! = " << result << "\n";
        }
    }
}

// Function to compute factorial for long
void factorial_long() {
    cout << "\n========== LONG FACTORIAL ==========\n";
    cout << "Size: " << sizeof(long) << " bytes\n";
    cout << "Max value: " << LONG_MAX << "\n";
    
    long result = 1;
    for (long n = 1; n <= 100; n++) {
        // Check for overflow before multiplication
        if (result > LONG_MAX / n) {
            cout << "Overflow would occur at n = " << n << "\n";
            cout << "Maximum n for long: " << (n - 1) << "\n";
            cout << (n-1) << "! = " << result << "\n";
            break;
        }
        
        result *= n;
        
        if (n <= 20 || n % 5 == 0) {
            cout << n << "! = " << result << "\n";
        }
    }
}

// Function to compute factorial for unsigned long
void factorial_unsigned_long() {
    cout << "\n========== UNSIGNED LONG FACTORIAL ==========\n";
    cout << "Size: " << sizeof(unsigned long) << " bytes\n";
    cout << "Max value: " << ULONG_MAX << "\n";
    
    unsigned long result = 1;
    for (unsigned long n = 1; n <= 100; n++) {
        // Check for overflow before multiplication
        if (result > ULONG_MAX / n) {
            cout << "Overflow would occur at n = " << n << "\n";
            cout << "Maximum n for unsigned long: " << (n - 1) << "\n";
            cout << (n-1) << "! = " << result << "\n";
            break;
        }
        
        result *= n;
        
        if (n <= 20 || n % 5 == 0) {
            cout << n << "! = " << result << "\n";
        }
    }
}

// Function to compute factorial for long long
void factorial_long_long() {
    cout << "\n========== LONG LONG FACTORIAL ==========\n";
    cout << "Size: " << sizeof(long long) << " bytes\n";
    cout << "Max value: " << LLONG_MAX << "\n";
    
    long long result = 1;
    for (long long n = 1; n <= 100; n++) {
        // Check for overflow before multiplication
        if (result > LLONG_MAX / n) {
            cout << "Overflow would occur at n = " << n << "\n";
            cout << "Maximum n for long long: " << (n - 1) << "\n";
            cout << (n-1) << "! = " << result << "\n";
            break;
        }
        
        result *= n;
        
        if (n <= 20 || n % 5 == 0) {
            cout << n << "! = " << result << "\n";
        }
    }
}

// Function to compute factorial for unsigned long long
void factorial_unsigned_long_long() {
    cout << "\n========== UNSIGNED LONG LONG FACTORIAL ==========\n";
    cout << "Size: " << sizeof(unsigned long long) << " bytes\n";
    cout << "Max value: " << ULLONG_MAX << "\n";
    
    unsigned long long result = 1;
    for (unsigned long long n = 1; n <= 100; n++) {
        // Check for overflow before multiplication
        if (result > ULLONG_MAX / n) {
            cout << "Overflow would occur at n = " << n << "\n";
            cout << "Maximum n for unsigned long long: " << (n - 1) << "\n";
            cout << (n-1) << "! = " << result << "\n";
            break;
        }
        
        result *= n;
        
        if (n <= 20 || n % 5 == 0) {
            cout << n << "! = " << result << "\n";
        }
    }
}

// Function to compute factorial for double (using logarithms)
void factorial_double() {
    cout << "\n========== DOUBLE FACTORIAL (using logarithms) ==========\n";
    cout << "Size: " << sizeof(double) << " bytes\n";
    cout << "Max value: " << DBL_MAX << "\n";
    cout << "Note: For double, we use log(factorial) to avoid overflow\n";
    cout << "Actual value = e^(log_factorial)\n\n";
    
    for (int n = 1; n <= 200; n++) {
        double log_factorial = 0.0;
        
        // Calculate log(n!)
        for (int i = 1; i <= n; i++) {
            log_factorial += log(i);
        }
        
        double log_value_e = log_factorial; // log base e
        
        // Check if this is representable in double
        if (log_value_e > log(DBL_MAX)) {
            cout << "Cannot represent in double after n = " << (n-1) << "\n";
            break;
        }
        
        if (n <= 20 || n % 10 == 0) {
            cout << "n = " << n << ": log(" << n << "!) = " << fixed << setprecision(2) << log_value_e << endl;
        }
    }
    
    cout << "\nFor reference, some factorials in scientific notation:\n";
    cout << "10! = 3.628800e+06\n";
    cout << "20! = 2.432902e+18\n";
    cout << "100! ≈ 9.333e+157\n";
    cout << "170! ≈ 7.257e+306 (near double max)\n";
    cout << "171! would overflow double\n";
}

// Comparison table
void comparison_table() {
    cout << "\n\n" << string(80, '=') << "\n";
    cout << "SUMMARY: MAXIMUM N FOR EACH DATA TYPE\n";
    cout << string(80, '=') << "\n\n";
    
    cout << setw(25) << "Data Type" 
         << setw(20) << "Size (bytes)"
         << setw(30) << "Max N for n!"
         << endl;
    cout << string(75, '-') << "\n";
    
    cout << setw(25) << "int"
         << setw(20) << sizeof(int)
         << setw(30) << "12"
         << endl;
    
    cout << setw(25) << "unsigned int"
         << setw(20) << sizeof(unsigned int)
         << setw(30) << "12"
         << endl;
    
    cout << setw(25) << "long"
         << setw(20) << sizeof(long)
         << setw(30) << "20 or 12" << " (depends on system)"
         << endl;
    
    cout << setw(25) << "unsigned long"
         << setw(20) << sizeof(unsigned long)
         << setw(30) << "20 or 12" << " (depends on system)"
         << endl;
    
    cout << setw(25) << "long long"
         << setw(20) << sizeof(long long)
         << setw(30) << "20"
         << endl;
    
    cout << setw(25) << "unsigned long long"
         << setw(20) << sizeof(unsigned long long)
         << setw(30) << "20"
         << endl;
    
    cout << setw(25) << "double"
         << setw(20) << sizeof(double)
         << setw(30) << "170 (with logarithms)"
         << endl;
    
    cout << "\n\nHOW TO HANDLE VERY LARGE FACTORIALS:\n";
    cout << "1. Use double with logarithms: log(n!) = log(1) + log(2) + ... + log(n)\n";
    cout << "2. Use string/vector to store individual digits\n";
    cout << "3. Use external libraries (Boost, GMP for arbitrary precision)\n";
}

int main() {
    cout << "\n" << string(80, '=') << "\n";
    cout << "FACTORIAL COMPUTATION WITH DIFFERENT DATA TYPES\n";
    cout << "Understanding Overflow and Data Type Limits\n";
    cout << string(80, '=') << "\n";
    
    factorial_int();
    factorial_unsigned_int();
    factorial_long();
    factorial_unsigned_long();
    factorial_long_long();
    factorial_unsigned_long_long();
    factorial_double();
    
    comparison_table();
    
    cout << "\n" << string(80, '=') << "\n";
    cout << "KEY INSIGHTS:\n";
    cout << string(80, '=') << "\n";
    cout << "- int, unsigned int: max n ≈ 12 (12! = 479,001,600)\n";
    cout << "- long, unsigned long: system dependent (64-bit: max n ≈ 20)\n";
    cout << "- long long: max n ≈ 20 (20! = 2,432,902,008,176,640,000)\n";
    cout << "- double: max n ≈ 170 (using logarithms)\n";
    cout << "- For arbitrary precision: use string or external libraries\n";
    cout << string(80, '=') << "\n\n";
    
    return 0;
}
