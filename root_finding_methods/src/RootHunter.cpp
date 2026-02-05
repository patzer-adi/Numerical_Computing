#include "../include/RootHunter.hpp"
#include "../utils/DisplayUtils.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

// Default Constructor - Standard detective protocols
RootHunter::RootHunter() {
    cout << "\n🔍 RootHunter initialized with DEFAULT settings!" << endl;
    patienceLevel = 0.0001;  // Default tolerance
    theGoldenNumber = 0.0;
    detectiveSteps = 0;
    leftSuspect = 0.0;
    rightSuspect = 0.0;
    cout << "📋 Patience Level (Tolerance) set to: " << patienceLevel << endl;
}

// Parameterized Constructor - Custom patience
RootHunter::RootHunter(double customPatience) {
    cout << "\n🔍 RootHunter initialized with CUSTOM settings!" << endl;
    patienceLevel = customPatience;
    theGoldenNumber = 0.0;
    detectiveSteps = 0;
    leftSuspect = 0.0;
    rightSuspect = 0.0;
    cout << "📋 Patience Level (Tolerance) set to: " << patienceLevel << endl;
}

// The Mystery Equation: f(x) = 4x³ - 3x
double RootHunter::theMysteryEquation(double x) {
    return 4*x*x*x - 3*x;
}

// Hunt for the crime scene - find interval containing root
void RootHunter::huntForCrimeScene() {
    DisplayUtils::printSectionHeader("STARTING THE MANHUNT FOR THE ROOT'S HIDEOUT...");
    
    cout << "\n🎯 The Mystery Equation: f(x) = 4x³ - 3x" << endl;
    cout << "🎯 Mission: Find where f(x) = 0" << endl;
    
    double x = 0.0;
    double stepSize = 0.5;  // Detective's stride
    bool crimeSceneFound = false;
    int searchAttempts = 0;
    
    cout << "\n🚶 Starting search from x = 0.0 with step size = " << stepSize << endl;
    DisplayUtils::printDivider();
    
    // Search in positive direction
    cout << "🔍 Searching in POSITIVE direction (→→→)..." << endl;
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
            cout << " ✅ JACKPOT! Sign change detected!" << endl;
            break;
        } else {
            cout << " ❌ Same sign, moving on..." << endl;
        }
    }
    
    // If not found, search in negative direction
    if (!crimeSceneFound) {
        cout << "\n🔍 Searching in NEGATIVE direction (←←←)..." << endl;
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
                cout << " ✅ JACKPOT! Sign change detected!" << endl;
                break;
            } else {
                cout << " ❌ Same sign, moving on..." << endl;
            }
        }
    }
    
    DisplayUtils::printDivider();
    
    if (!crimeSceneFound) {
        cout << "❌ CASE COLD! Could not find suitable interval!" << endl;
        cout << "💡 Suggestion: Expand search range or check the equation." << endl;
    } else {
        cout << "🎉 CRIME SCENE LOCATED after " << searchAttempts << " search attempts!" << endl;
        cout << "📍 Interval: [" << setprecision(4) << leftSuspect << ", " << rightSuspect << "]" << endl;
        cout << "📊 f(" << leftSuspect << ") = " << theMysteryEquation(leftSuspect) << endl;
        cout << "📊 f(" << rightSuspect << ") = " << theMysteryEquation(rightSuspect) << endl;
        cout << "✓ Sign difference confirmed: Root is hiding in this interval!" << endl;
    }
}

// The main investigation - Bisection Method
void RootHunter::solveTheMystery() {
    DisplayUtils::printSectionHeader("BEGINNING THE INTERROGATION PROCESS...");
    
    double leftValue = theMysteryEquation(leftSuspect);
    double rightValue = theMysteryEquation(rightSuspect);
    
    // Validate interval
    cout << "\n🔒 Pre-investigation check..." << endl;
    cout << "   Left suspect value: f(" << leftSuspect << ") = " << leftValue << endl;
    cout << "   Right suspect value: f(" << rightSuspect << ") = " << rightValue << endl;
    
    if (leftValue * rightValue > 0) {
        cout << "\n❌ INVESTIGATION FAILED!" << endl;
        cout << "   Both suspects have the same sign!" << endl;
        cout << "   No root guaranteed in this interval." << endl;
        return;
    }
    
    cout << "✓ Both suspects have opposite signs. Investigation can proceed!" << endl;
    DisplayUtils::printDivider();
    cout << "🔍 INTERROGATION LOG:" << endl;
    DisplayUtils::printDivider();
    
    double middleman;  // Midpoint
    double middlemanValue; // Function value at midpoint
    
    // Keep bisecting until tolerance is reached
    while ((rightSuspect - leftSuspect) >= patienceLevel) {
        // Calculate midpoint
        middleman = (leftSuspect + rightSuspect) / 2.0;
        middlemanValue = theMysteryEquation(middleman);
        
        detectiveSteps++;
        
        cout << "Round " << detectiveSteps << ":" << endl;
        cout << "   🎯 Current interval: [" << setprecision(6) << leftSuspect 
             << ", " << rightSuspect << "]" << endl;
        cout << "   📏 Interval width: " << (rightSuspect - leftSuspect) << endl;
        cout << "   🔍 Middleman position: " << middleman << endl;
        cout << "   💡 f(middleman) = " << middlemanValue << endl;
        
        // Check if we found exact root
        if (fabs(middlemanValue) < 1e-10) {
            cout << "   🎊 BULLSEYE! Found exact root!" << endl;
            break;
        }
        
        // Decide which half to keep
        double leftSide = theMysteryEquation(leftSuspect) * middlemanValue;
        
        if (leftSide < 0) {
            cout << "   ➡️  Root is in LEFT half [" << leftSuspect << ", " << middleman << "]" << endl;
            rightSuspect = middleman;
        } else {
            cout << "   ➡️  Root is in RIGHT half [" << middleman << ", " << rightSuspect << "]" << endl;
            leftSuspect = middleman;
        }
        
        cout << "   ✓ Narrowing down...\n" << endl;
    }
    
    // Calculate final root
    theGoldenNumber = (leftSuspect + rightSuspect) / 2.0;
    
    DisplayUtils::printDivider();
    cout << "✅ Investigation converged! Patience threshold reached." << endl;
}

// Present the evidence report
void RootHunter::presentEvidenceReport() {
    DisplayUtils::printSectionHeader("FINAL INVESTIGATION REPORT");
    
    cout << "\n🎯 THE MYSTERY EQUATION: f(x) = 4x³ - 3x" << endl;
    DisplayUtils::printDivider();
    cout << "\n🏆 THE GOLDEN NUMBER (Root): " << setprecision(10) << theGoldenNumber << endl;
    cout << "🔢 Detective Steps Taken: " << detectiveSteps << endl;
    cout << "🎯 Precision Achieved: " << patienceLevel << endl;
    cout << "✨ Verification: f(" << theGoldenNumber << ") = " 
         << setprecision(12) << theMysteryEquation(theGoldenNumber) << endl;
    
    DisplayUtils::printDivider();
    cout << "🎉 CASE CLOSED! The root has been successfully captured!" << endl;
    DisplayUtils::printDivider();
}
