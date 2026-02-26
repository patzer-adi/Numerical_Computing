#pragma once

#include <string>
#include "../include/Matrix.hpp"

namespace Input {

    Matrix readMatrixFromConsole();

    Matrix readMatrixFromFile(const std::string& filename);

    Matrix readAugmentedFromFile(const std::string& filename);

    Matrix readFromLeftRightFiles(const std::string& leftFile,
                                  const std::string& rightFile);
}
