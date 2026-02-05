#ifndef INPUTUTILS_HPP
#define INPUTUTILS_HPP

// Utility class for input validation
class InputUtils {
public:
    // Get a validated menu choice (1 or 2)
    static int getMenuChoice();
    
    // Get a validated tolerance value (must be positive)
    static double getTolerance();
};

#endif
