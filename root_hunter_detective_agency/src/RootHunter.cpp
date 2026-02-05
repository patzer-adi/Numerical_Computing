#include "../include/RootHunter.hpp"
#include "../utils/DisplayUtils.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <thread>        // Add this for sleep
#include <chrono>        // Add this for time duration
using namespace std;

// Default Constructor - Standard detective protocols
RootHunter::RootHunter() {
    cout << "\nRootHunter initialized with DEFAULT settings!" << endl;
    patienceLevel = 0.0001;  // Default tolerance
    theGoldenNumber = 0.0;
    detectiveSteps = 0;
    leftSuspect = 0.0;
    rightSuspect = 0.0;
    cout << "Patience Level (Tolerance) set to: " << patienceLevel << endl;
}

// Parameterized Constructor - Custom patience
RootHunter::RootHunter(double customPatience) {
    cout << "\nRootHunter initialized with CUSTOM settings!" << endl;
    patienceLevel = customPatience;
    theGoldenNumber = 0.0;
    detectiveSteps = 0;
    leftSuspect = 0.0;
    rightSuspect = 0.0;
    cout << "Patience Level (Tolerance) set to: " << patienceLevel << endl;
}

// The Mystery Equation: f(x) = 4x³ - 3x
double RootHunter::theMysteryEquation(double x) {
    return 4*x*x*x - 3*x;
}

// Hunt for the crime scene - find interval containing root
void RootHunter::huntForCrimeScene() {
    DisplayUtils::printSectionHeader("STARTING THE MANHUNT FOR THE ROOT'S HIDEOUT...");

    cout << "\nThe Mystery Equation: f(x) = 4x³ - 3x" << endl;
    cout << "Mission: Find where f(x) = 0" << endl;

    double x = 0.0;
    double stepSize = 0.5;  // Detective's stride
    bool crimeSceneFound = false;
    int searchAttempts = 0;

    cout << "\n🔍 Starting search from x = 0.0 with step size = " << stepSize << endl;
    DisplayUtils::printDivider();

    // Search in positive direction
    cout << "Searching in POSITIVE direction (→→→)..." << endl;
    for (x = 0.0; x <= 10.0; x += stepSize) {
        searchAttempts++;
        double suspectValue1 = theMysteryEquation(x);
        double suspectValue2 = theMysteryEquation(x + stepSize);

        cout << "   Step " << searchAttempts << ": Checking [" << fixed << setprecision(2)
        << x << ", " << (x + stepSize) << "] → f(" << x << ")="
        << setprecision(4) << suspectValue1 << ", f(" << (x + stepSize) << ")="
        << suspectValue2;

        // Check for sign change (root exists!)
        if (suspectValue1 * suspectValue2 < 0) {
            leftSuspect = x;
            rightSuspect = x + stepSize;
            crimeSceneFound = true;
            cout << " JACKPOT! Sign change detected!" << endl;
            break;
        } else {
            cout << " Same sign, moving on..." << endl;
        }
    }

    // If not found, search in negative direction
    if (!crimeSceneFound) {
        cout << "\nSearching in NEGATIVE direction (←←←)..." << endl;
        for (x = -stepSize; x >= -10.0; x -= stepSize) {
            searchAttempts++;
            double suspectValue1 = theMysteryEquation(x);
            double suspectValue2 = theMysteryEquation(x - stepSize);

            cout << "   Step " << searchAttempts << ": Checking [" << fixed << setprecision(2)
            << (x - stepSize) << ", " << x << "] → f(" << (x - stepSize) << ")="
            << setprecision(4) << suspectValue2 << ", f(" << x << ")="
            << suspectValue1;

            if (suspectValue1 * suspectValue2 < 0) {
                leftSuspect = x - stepSize;
                rightSuspect = x;
                crimeSceneFound = true;
                cout << " JACKPOT! Sign change detected!" << endl;
                break;
            } else {
                cout << "Same sign, moving on..." << endl;
            }
        }
    }

    DisplayUtils::printDivider();

    if (!crimeSceneFound) {
        cout << "CASE COLD! Could not find suitable interval!" << endl;
        cout << "Suggestion: Expand search range or check the equation." << endl;
    } else {
        cout << "CRIME SCENE LOCATED after " << searchAttempts << " search attempts!" << endl;
        cout << "Interval: [" <<  leftSuspect << ", " << rightSuspect << "]" << endl;
        cout << "f(" << leftSuspect << ") = " << theMysteryEquation(leftSuspect) << endl;
        cout << "f(" << rightSuspect << ") = " << theMysteryEquation(rightSuspect) << endl;
        cout << "Sign difference confirmed: Root is hiding in this interval!" << endl;
    }
}

// Hunt for the crime scene - find interval containing root
void RootHunter::solveTheMystery() {
    DisplayUtils::printSectionHeader("BEGINNING THE INTERROGATION PROCESS...");

    double leftValue = theMysteryEquation(leftSuspect);
    double rightValue = theMysteryEquation(rightSuspect);

    // Validate interval
    cout << "\nPre-investigation check..." << endl;
    cout << "   Left suspect value: f(" << leftSuspect << ") = " << leftValue << endl;
    cout << "   Right suspect value: f(" << rightSuspect << ") = " << rightValue << endl;

    if (leftValue * rightValue > 0) {
        cout << "\nINVESTIGATION FAILED!" << endl;
        cout << "   Both suspects have the same sign!" << endl;
        cout << "   No root guaranteed in this interval." << endl;
        return;
    }

    cout << "Both suspects have opposite signs. Investigation can proceed!" << endl;
    DisplayUtils::printDivider();
    cout << "INTERROGATION LOG:" << endl;
    DisplayUtils::printDivider();

    double c;   // Midpoint
    double fc;  // Function value at midpoint

    // Keep bisecting until tolerance is reached
    while ((rightSuspect - leftSuspect) >= patienceLevel) {
        // Calculate midpoint
        c = (leftSuspect + rightSuspect) / 2.0;
        fc = theMysteryEquation(c);

        detectiveSteps++;

        // Print detailed round information
        cout << "Round " << detectiveSteps << ":" << endl;
        cout << "Current interval: [" << setprecision(6) << leftSuspect
        << ", " << rightSuspect << "]" << endl;
        cout << "Interval width: " << (rightSuspect - leftSuspect) << endl;
        cout << "Middleman position: " << c << endl;
        cout << "f(middleman) = " << fc << endl;

        // Check if we found exact root
        if (fc == 0.0) {
            cout << "BULLSEYE! Found exact root!" << endl;
            break;
        }

        // Decide which half to keep
        if (theMysteryEquation(leftSuspect) * fc < 0) {
            cout << "Root is in LEFT half [" << leftSuspect << ", " << c << "]" << endl;
            rightSuspect = c;  // Root is in left half
        } else {
            cout << "Root is in RIGHT half [" << c << ", " << rightSuspect << "]" << endl;
            leftSuspect = c;   // Root is in right half
        }

        cout << "  Narrowing down...\n" << endl;

        // ⏱️ PAUSE FOR 0.5 SECONDS
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    // Calculate final root
    theGoldenNumber = (leftSuspect + rightSuspect) / 2.0;

    DisplayUtils::printDivider();
    cout << "Investigation converged! Patience threshold reached." << endl;
}


// Present the evidence report
void RootHunter::presentEvidenceReport() {
    DisplayUtils::printSectionHeader("FINAL INVESTIGATION REPORT");
    
    cout << "\nTHE MYSTERY EQUATION: f(x) = 4x³ - 3x" << endl;
    DisplayUtils::printDivider();
    cout << "\nTHE GOLDEN NUMBER (Root): " << setprecision(10) << theGoldenNumber << endl;
    cout << "Detective Steps Taken: " << detectiveSteps << endl;
    cout << "Precision Achieved: " << patienceLevel << endl;
    cout << "Verification: f(" << theGoldenNumber << ") = "
         << setprecision(12) << theMysteryEquation(theGoldenNumber) << endl;
    
    DisplayUtils::printDivider();
    cout << " CASE CLOSED! The root has been successfully captured!" << endl;
    DisplayUtils::printDivider();
}
