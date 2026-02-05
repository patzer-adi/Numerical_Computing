#include "complexClass_header.hpp"
#include <iostream>

Complex::Complex()
{
    real = 0;
    imag = 0;
}

// Parameterized Constructor
Complex::Complex(float r, float i)
    {
        real = r;
        imag = i;
    }

// Input function
void Complex::input()
    {
        std::cout << "Krupiya saccha part enter kijiye: ";
        std::cin >> real;
        std::cout << "Krupiya khayali enter kijiye: ";
        std::cin >> imag;
    }

// Display function
void Complex::display()
    {
        std::cout << real;
        if (imag >= 0)
            std::cout << " + " << imag << "i" << std::endl;
        else
            std::cout << " - " << -imag << "i" << std::endl;
    }

// Getter for real part
float Complex::getReal()
    {
        return real;
    }

// Getter for imaginary part
float Complex::getImag()
    {
        return imag;
    }

// Addition
Complex Complex::add(Complex obj)
    {
        Complex temp;
        temp.real = real + obj.real;
        temp.imag = imag + obj.imag;
        return temp;
    }

// Subtraction
// Complex Complex::subtract(Complex obj)
//     {
//         Complex temp;
//         temp.real = real - obj.real;
//         temp.imag = imag - obj.imag;
//         return temp;
//     }

Complex Complex::subtract(Complex c)
    {
        return Complex(real - c.real, imag - c.imag);
    }

void Complex::display()
    {
        std::cout<< real << " + " << (imag < 0 ? -imag : imag) << "i" << std::endl;
    }

// Multiplication
Complex Complex::multiply(Complex obj)
    {
        Complex temp;
        temp.real = (real * obj.real) - (imag * obj.imag);
        temp.imag = (real * obj.imag) + (imag * obj.real);
        return temp;
    }

// Division
Complex Complex::divide(Complex obj)
    {
        Complex temp;
        float denominator = (obj.real * obj.real) + (obj.imag * obj.imag);
        temp.real = ((real * obj.real) + (imag * obj.imag)) / denominator;
        temp.imag = ((imag * obj.real) - (real * obj.imag)) / denominator;
        return temp;
    }

// Conjugate
Complex Complex::conjugate()
    {
        Complex temp;
        temp.real = real;
        temp.imag = -imag;
        return temp;
    }  

// Norm
float Complex:: Norm()
{
    return sqrt((real * real) + (imag * imag));
}

