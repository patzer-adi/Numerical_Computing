#ifndef ROOTHUNTER_HPP
#define ROOTHUNTER_HPP

// The RootHunter - A Mathematical Detective Agency
class RootHunter {
private:
    double leftSuspect;      // Lower bound (left suspect interval)
    double rightSuspect;     // Upper bound (right suspect interval)
    double patienceLevel;    // Tolerance for stopping
    double theGoldenNumber;  // Final root value
    int detectiveSteps;      // Number of iterations taken

public:
    // Default Constructor - Uses standard detective protocols
    RootHunter();
    
    // Parameterized Constructor - Custom patience level
    RootHunter(double customPatience);
    
    // The Mystery Function we're investigating
    // f(x) = 4x^3-3x (The cubic culprit)
    double theMysteryEquation(double x);
    
    // Hunt for the crime scene (interval containing root)
    void huntForCrimeScene();
    
    // The main investigation - bisection method
    void solveTheMystery();
    
    // Present the case file
    void presentEvidenceReport();
};

#endif
