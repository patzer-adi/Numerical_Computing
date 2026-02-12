#include "complexClass_header.hpp"
#include <iostream>
#include <cmath>   // Needed for sqrt
#include <ostream>

Complex::Complex()
{
    real = 0;
    imag = 0;
}

Complex::Complex(float r, float i)
    {
        real = r;
        imag = i;
    }

void Complex::input()
    {
        std::cout << "Krupiya saccha part enter kijiye: ";
        std::cin >> real;
        std::cout << "Krupiya khayali enter kijiye: ";
        std::cin >> imag;
    }

void Complex::display()
    {
        std::cout << real;
        if (imag >= 0)
            std::cout << " + " << imag << "i" << std::endl;
        else
            std::cout << " - " << -imag << "i" << std::endl;
    }

float Complex::getReal()
    {
        return real;
    }

float Complex::getImag()
    {
        return imag;
    }

Complex Complex::add(Complex obj)
    {
        Complex temp;
        temp.real = real + obj.real;
        temp.imag = imag + obj.imag;
        return temp;
    }

    Complex Complex::operator+(const Complex& obj)
    {
        return Complex(real + obj.real, imag + obj.imag);
    }


Complex Complex::subtract(Complex c)
    {
        return Complex(real - c.real, imag - c.imag);
    }

    Complex Complex::operator-(const Complex& obj)
    {
        return Complex(real - obj.real, imag - obj.imag);
    }


// void Complex::display()
//     {
//         std::cout<< real << " + " << (imag < 0 ? -imag : imag) << "i" << std::endl;
//     }

// Multiplication
Complex Complex::multiply(Complex obj)
    {
        Complex temp;
        temp.real = (real * obj.real) - (imag * obj.imag);
        temp.imag = (real * obj.imag) + (imag * obj.real);
        return temp;
    }

    Complex Complex::operator*(const Complex& obj)
    {
        float r = (real * obj.real) - (imag * obj.imag);
        float i = (real * obj.imag) + (imag * obj.real);
        return Complex(r, i);
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

    Complex Complex::operator/(const Complex& obj)
    {
        float denominator = (obj.real * obj.real) + (obj.imag * obj.imag);

        float r = ((real * obj.real) + (imag * obj.imag)) / denominator;
        float i = ((imag * obj.real) - (real * obj.imag)) / denominator;

        return Complex(r, i);
    }
/*
    std::ostream& operator<<(std::ostream& out, const Complex& c)
    {
        out << c.real;
        if (c.imag >= 0)ostream
            out << " + " << c.imag << "i";
        else
            out << " - " ostream<< -c.imag << "i";
        return out;
    }*/


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

