#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>

using namespace std;

// Generalized Geometric series: sum of base^(-k) from k=1 to n
// Formula: sum = base^(-1) + base^(-2) + ... + base^(-n)
//         = (1 - base^(-n)) / (base - 1)

void compute_sum_float() {
    cout << "\n========== USING FLOAT ==========\n";
    cout << "Size: " << sizeof(float) << " bytes\n";
    cout << "Precision: ~6-7 significant digits\n\n";
    
    int n;
    float base;
    cout << "Enter value of n (sum from k=1 to n): ";
    cin >> n;
    cout << "Enter base (e.g., 10 for 10^(-k)): ";
    cin >> base;
    
    if (base <= 0) {
        cout << "Base must be positive! Using base = 10\n";
        base = 10.0f;
    }
    if (base == 1) {
        cout << "Base cannot be 1! Using base = 10\n";
        base = 10.0f;
    }
    
    float sum = 0.0f;
    cout << "\nIndividual terms:\n";
    cout << fixed << setprecision(10);
    
    for (int k = 1; k <= n; k++) {
        float term = pow(base, -k);
        sum += term;
        
        if (k <= 10 || k == n) {
            cout << "k=" << k << ": " << base << "^(-" << k << ") = " << term 
                 << ", Running sum = " << sum << "\n";
        } else if (k == 11) {
            cout << "...\n";
        }
    }
    
    // Using formula: (1 - base^(-n)) / (base - 1)
    float sum_formula = (1.0f - pow(base, -n)) / (base - 1.0f);
    
    cout << "\nUsing loop: " << setprecision(10) << sum << "\n";
    cout << "Using formula (1 - base^(-n)) / (base - 1): " << sum_formula << "\n";
    cout << "Difference: " << abs(sum - sum_formula) << "\n";
}

void compute_sum_double() {
    cout << "\n========== USING DOUBLE ==========\n";
    cout << "Size: " << sizeof(double) << " bytes\n";
    cout << "Precision: ~15-17 significant digits\n\n";
    
    int n;
    double base;
    cout << "Enter value of n (sum from k=1 to n): ";
    cin >> n;
    cout << "Enter base (e.g., 10 for 10^(-k)): ";
    cin >> base;
    
    if (base <= 0) {
        cout << "Base must be positive! Using base = 10\n";
        base = 10.0;
    }
    if (base == 1) {
        cout << "Base cannot be 1! Using base = 10\n";
        base = 10.0;
    }
    
    double sum = 0.0;
    cout << "\nIndividual terms:\n";
    cout << fixed << setprecision(15);
    
    for (int k = 1; k <= n; k++) {
        double term = pow(base, -k);
        sum += term;
        
        if (k <= 10 || k == n) {
            cout << "k=" << k << ": " << base << "^(-" << k << ") = " << term 
                 << ", Running sum = " << sum << "\n";
        } else if (k == 11) {
            cout << "...\n";
        }
    }
    
    // Using formula: (1 - base^(-n)) / (base - 1)
    double sum_formula = (1.0 - pow(base, -n)) / (base - 1.0);
    
    cout << "\nUsing loop: " << setprecision(15) << sum << "\n";
    cout << "Using formula (1 - base^(-n)) / (base - 1): " << sum_formula << "\n";
    cout << "Difference: " << abs(sum - sum_formula) << "\n";
}

void compute_sum_long_double() {
    cout << "\n========== USING LONG DOUBLE ==========\n";
    cout << "Size: " << sizeof(long double) << " bytes\n";
    cout << "Precision: ~18-19 significant digits (platform dependent)\n\n";
    
    int n;
    long double base;
    cout << "Enter value of n (sum from k=1 to n): ";
    cin >> n;
    cout << "Enter base (e.g., 10 for 10^(-k)): ";
    cin >> base;
    
    if (base <= 0) {
        cout << "Base must be positive! Using base = 10\n";
        base = 10.0L;
    }
    if (base == 1) {
        cout << "Base cannot be 1! Using base = 10\n";
        base = 10.0L;
    }
    
    long double sum = 0.0L;
    cout << "\nIndividual terms:\n";
    cout << fixed << setprecision(18);
    
    for (int k = 1; k <= n; k++) {
        long double term = powl(base, -k);
        sum += term;
        
        if (k <= 10 || k == n) {
            cout << "k=" << k << ": " << base << "^(-" << k << ") = " << term 
                 << ", Running sum = " << sum << "\n";
        } else if (k == 11) {
            cout << "...\n";
        }
    }
    
    // Using formula: (1 - base^(-n)) / (base - 1)
    long double sum_formula = (1.0L - powl(base, -n)) / (base - 1.0L);
    
    cout << "\nUsing loop: " << setprecision(18) << sum << "\n";
    cout << "Using formula (1 - base^(-n)) / (base - 1): " << sum_formula << "\n";
    cout << "Difference: " << fabsl(sum - sum_formula) << "\n";
}

void compute_specific_sum() {
    cout << "\n========== CUSTOM INPUT: SUM OF base^(-k) ==========\n";
    cout << "This computes: base^(-1) + base^(-2) + ... + base^(-n)\n\n";
    
    int n;
    double base;
    cout << "Enter value of n: ";
    cin >> n;
    cout << "Enter base: ";
    cin >> base;
    
    if (base <= 0) {
        cout << "Base must be positive! Using base = 10\n";
        base = 10.0;
    }
    if (base == 1) {
        cout << "Base cannot be 1! Using base = 10\n";
        base = 10.0;
    }
    
    double sum = 0.0;
    cout << fixed << setprecision(18);
    cout << "\nIndividual terms:\n";
    
    for (int k = 1; k <= n; k++) {
        double term = pow(base, -k);
        sum += term;
        cout << "k=" << k << ": " << base << "^(-" << k << ") = " << term 
             << ", Running sum = " << sum << "\n";
    }
    
    // Using formula: (1 - base^(-n)) / (base - 1)
    double sum_formula = (1.0 / (base - 1.0)) * (1.0 - pow(base, -n));
    
    cout << "\n--- RESULTS ---\n";
    cout << "Final Sum (using loop): " << setprecision(18) << sum << "\n";
    cout << "Final Sum (using formula): " << sum_formula << "\n";
    cout << "Difference: " << abs(sum - sum_formula) << "\n";
    
    cout << "\nFormula used: (1 - " << base << "^(-" << n << ")) / (" << base << " - 1)\n";
    cout << "           = (1 - " << pow(base, -n) << ") / " << (base - 1.0) << "\n";
    cout << "           = " << (1.0 - pow(base, -n)) << " / " << (base - 1.0) << "\n";
    cout << "           = " << sum_formula << "\n";
}

void analysis() {
    cout << "\n\n" << string(80, '=') << "\n";
    cout << "MATHEMATICAL ANALYSIS\n";
    cout << string(80, '=') << "\n\n";
    
    cout << "Given Series: base^(-1) + base^(-2) + ... + base^(-n)\n\n";
    
    cout << "This is a Geometric Series with:\n";
    cout << "  First term (a) = base^(-1) = 1/base\n";
    cout << "  Common ratio (r) = base^(-1) = 1/base\n";
    cout << "  Number of terms = n\n\n";
    
    cout << "Formula for geometric series: S_n = a(1 - r^n)/(1 - r)\n\n";
    
    cout << "S_n = (1/base) * (1 - (1/base)^n) / (1 - 1/base)\n";
    cout << "    = (1/base) * (1 - 1/base^n) / ((base-1)/base)\n";
    cout << "    = (1 - 1/base^n) / (base - 1)\n";
    cout << "    = (1 - base^(-n)) / (base - 1)\n\n";
    
    cout << "As n increases towards infinity:\n";
    cout << "  base^(-n) approaches 0\n";
    cout << "  Sum approaches 1/(base - 1)\n\n";
    
    cout << "EXAMPLES:\n";
    cout << "1. For base = 10:\n";
    cout << "   Sum (n=∞) = 1/(10-1) = 1/9 ≈ 0.1111111...\n\n";
    
    cout << "2. For base = 2:\n";
    cout << "   Sum (n=∞) = 1/(2-1) = 1/1 = 1\n\n";
    
    cout << "3. For base = 3:\n";
    cout << "   Sum (n=∞) = 1/(3-1) = 1/2 = 0.5\n\n";
}

void comparison_large_n() {
    cout << "\n" << string(80, '=') << "\n";
    cout << "SUM VALUES FOR DIFFERENT n VALUES (BASE = 10)\n";
    cout << string(80, '=') << "\n\n";
    
    double base = 10.0;
    
    cout << setw(5) << "n" 
         << setw(20) << "10^(-n)"
         << setw(25) << "Sum (using formula)"
         << endl;
    cout << string(50, '-') << "\n";
    
    cout << fixed << setprecision(15);
    
    for (int n = 1; n <= 20; n++) {
        double term = pow(base, -n);
        double sum = (1.0 - pow(base, -n)) / (base - 1.0);
        
        cout << setw(5) << n 
             << setw(20) << term
             << setw(25) << sum
             << endl;
    }
    
    cout << "\nAs you can see, the sum converges to 1/9 ≈ 0.111111...\n";
    
    cout << "\n\nCOMPARISON WITH DIFFERENT BASES\n";
    cout << string(80, '=') << "\n\n";
    
    cout << setw(10) << "n" 
         << setw(18) << "Base=2" 
         << setw(18) << "Base=10"
         << setw(18) << "Base=3"
         << endl;
    cout << string(64, '-') << "\n";
    
    for (int n = 1; n <= 10; n++) {
        double sum2 = (1.0 - pow(2.0, -n)) / (2.0 - 1.0);
        double sum10 = (1.0 - pow(10.0, -n)) / (10.0 - 1.0);
        double sum3 = (1.0 - pow(3.0, -n)) / (3.0 - 1.0);
        
        cout << fixed << setprecision(10)
             << setw(10) << n 
             << setw(18) << sum2
             << setw(18) << sum10
             << setw(18) << sum3
             << endl;
    }
    
    cout << "\nInfinite sums:\n";
    cout << "Base 2: Sum(∞) = 1/(2-1) = 1.0\n";
    cout << "Base 10: Sum(∞) = 1/(10-1) ≈ 0.1111111\n";
    cout << "Base 3: Sum(∞) = 1/(3-1) = 0.5\n";
}

int main() {
    cout << "\n" << string(80, '=') << "\n";
    cout << "GEOMETRIC SERIES: SUM OF base^(-k) FROM k=1 TO n\n";
    cout << "Formula: Sum = (1 - base^(-n)) / (base - 1)\n";
    cout << "Infinite Sum: Sum(∞) = 1 / (base - 1)  [for base > 1]\n";
    cout << string(80, '=') << "\n";
    
    int choice;
    
    while (true) {
        cout << "\n\nMENU:\n";
        cout << "1. Compute sum using float (input n and base)\n";
        cout << "2. Compute sum using double (input n and base)\n";
        cout << "3. Compute sum using long double (input n and base)\n";
        cout << "4. Custom input for any n and base\n";
        cout << "5. Mathematical analysis\n";
        cout << "6. Comparison table for different n and base values\n";
        cout << "7. Exit\n";
        cout << "Enter your choice (1-7): ";
        
        cin >> choice;
        
        if (choice == 1) {
            compute_sum_float();
        } else if (choice == 2) {
            compute_sum_double();
        } else if (choice == 3) {
            compute_sum_long_double();
        } else if (choice == 4) {
            compute_specific_sum();
        } else if (choice == 5) {
            analysis();
        } else if (choice == 6) {
            comparison_large_n();
        } else if (choice == 7) {
            cout << "\nExiting...\n";
            break;
        } else {
            cout << "Invalid choice! Please try again.\n";
        }
    }
    
    return 0;
}
