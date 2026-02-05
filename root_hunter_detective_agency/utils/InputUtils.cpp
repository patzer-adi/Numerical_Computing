#include "InputUtils.hpp"
#include <iostream>
#include <limits>
using namespace std;

int InputUtils::getMenuChoice() {
    int choice;
    cout << "Enter your choice (1 or 2): ";
    cin >> choice;
    
    // Validate input
    while (cin.fail() || (choice != 1 && choice != 2)) {
        cin.clear(); // Clear error flags
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
        cout << " Invalid choice! Please enter 1 or 2: ";
        cin >> choice;
    }
    
    return choice;
}

double InputUtils::getTolerance() {
    double customPatience;
    
    cout << "\nCustom Investigation Mode Selected!" << endl;
    cout << "Patience Level = How close we need to get to the root" << endl;
    cout << "   (Smaller value = More precision, More steps)" << endl;
    cout << "   Recommended: 0.0001 to 0.000001" << endl;
    cout << "\nEnter your desired Patience Level (Tolerance): ";
    cin >> customPatience;
    
    // Validate tolerance input
    while (cin.fail() || customPatience <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid! Patience must be a positive number: ";
        cin >> customPatience;
    }
    
    return customPatience;
}
