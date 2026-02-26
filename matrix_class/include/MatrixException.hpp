#ifndef MATRIXEXCEPTION_HPP
#define MATRIXEXCEPTION_HPP

#include <iostream>
#include <string>
using namespace std;

class MatrixException {
private:
  string message;

public:
  MatrixException(string msg) { message = msg; }

  string getMessage() { return message; }

  void printError() { cout << "Error: " << message << endl; }
};

#endif
