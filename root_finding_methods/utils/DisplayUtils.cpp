#include "DisplayUtils.hpp"
#include <iostream>
using namespace std;

void DisplayUtils::printSectionHeader(const string& title) {
    cout << "\n╔════════════════════════════════════════════════════════╗" << endl;
    cout << "║  " << title;
    // Pad to make it look nice
    int padding = 56 - title.length();
    for (int i = 0; i < padding; i++) cout << " ";
    cout << "║" << endl;
    cout << "╚════════════════════════════════════════════════════════╝" << endl;
}

void DisplayUtils::printDivider() {
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
}

void DisplayUtils::printWelcomeBanner() {
    cout << "\n╔══════════════════════════════════════════════════════════╗" << endl;
    cout << "║                                                          ║" << endl;
    cout << "║        🕵️  WELCOME TO THE ROOT HUNTER AGENCY 🕵️         ║" << endl;
    cout << "║                                                          ║" << endl;
    cout << "║         \"We Find Roots Others Only Dream Of\"            ║" << endl;
    cout << "║                                                          ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════╝" << endl;
    
    cout << "\n📖 Today's Case: The Mysterious Cubic Equation" << endl;
    cout << "   f(x) = 4x³ - 3x" << endl;
    cout << "   Mission: Find where this function crosses zero!\n" << endl;
}

void DisplayUtils::printMenu() {
    printDivider();
    cout << "🎮 Choose Your Detective Mode:" << endl;
    cout << "   1️⃣  Quick Investigation (Default Settings)" << endl;
    cout << "   2️⃣  Custom Investigation (Your Own Patience Level)" << endl;
    printDivider();
}

void DisplayUtils::printInvestigationStart() {
    cout << "\n";
    printDivider();
    cout << "🚨 INVESTIGATION BEGINNING IN 3... 2... 1..." << endl;
    printDivider();
    cout << endl;
}

void DisplayUtils::printGoodbye() {
    cout << "\n👋 Thank you for using Root Hunter Agency!" << endl;
    cout << "   Come back when you have another mathematical mystery! 🔍\n" << endl;
}
