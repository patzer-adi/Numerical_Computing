#pragma once
#include <exception>
#include <string>

class MatrixException : public std::exception {
private:
    std::string message;

public:
    MatrixException(const std::string& msg);
    const char* what() const noexcept override;
};
