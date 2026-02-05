#include<iostream>
#include "complexClass_header.hpp"
using namespace std;

int main()
{
    Complex c1(2.3, 4.5);
    Complex c2;
    cout << "\nFirst Complex part as Parameterized Constructor:\n";

    cout << "\nEnter second complex number:\n";
    c2.input();

    cout << "\nFirst Complex Number: ";
    c1.display();

    cout << "Second Complex Number: ";
    c2.display();

    // Addition
    Complex sum = c1.add(c2);
    cout << "Sum: ";
    sum.display();

    // Subtraction
    Complex difference = c1.subtract(c2);
    cout << "Difference: ";
    difference.display();

    // Multiplication
    Complex product = c1.multiply(c2);
    cout << "Product: ";
    product.display();

    // Division
    Complex quotient = c1.divide(c2);
    cout << "Quotient: ";
    quotient.display(); 

    // Conjugate of first complex number
    Complex conjugateC1 = c1.conjugate();
    cout << "Conjugate of First Complex Number: ";
    conjugateC1.display();
    
    // Conjugate of second complex number
    Complex conjugateC2 = c2.conjugate();
    cout << "Conjugate of Second Complex Number: ";
    conjugateC2.display();

    // Norms
    cout << "Norm of First Complex Number: " << c1.Norm() << endl;
    cout << "Norm of Second Complex Number: " << c2.Norm() << endl;

    return 0;
}