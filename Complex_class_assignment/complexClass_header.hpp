class Complex
{
    private:
        float real;
        float imag;

    public:
        Complex();
        Complex(float r, float i);
        void input();
        void display();
        float getReal();
        float getImag();
        Complex add(Complex obj);
        Complex multiply(Complex obj);
        Complex subtract(Complex obj);
        Complex divide(Complex obj);
        Complex conjugate();
        float Norm();
};