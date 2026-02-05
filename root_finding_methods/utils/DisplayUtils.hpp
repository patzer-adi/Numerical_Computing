#ifndef DISPLAYUTILS_HPP
#define DISPLAYUTILS_HPP

#include <string>

// Utility class for display formatting
class DisplayUtils {
public:
    // Print a fancy section header
    static void printSectionHeader(const std::string& title);
    
    // Print a divider line
    static void printDivider();
    
    // Print the welcome banner
    static void printWelcomeBanner();
    
    // Print the menu
    static void printMenu();
    
    // Print investigation start message
    static void printInvestigationStart();
    
    // Print goodbye message
    static void printGoodbye();
};

#endif
