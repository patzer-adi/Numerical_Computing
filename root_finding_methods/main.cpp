#include "include/RootHunter.hpp"
#include "utils/DisplayUtils.hpp"
#include "utils/InputUtils.hpp"
#include <iostream>
using namespace std;

int main() {
    DisplayUtils::printWelcomeBanner();
    
    DisplayUtils::printMenu();
    
    int choice = InputUtils::getMenuChoice();
    
    RootHunter* detective;
    
    if (choice == 1) {
        cout << "\n🚀 Initiating Quick Investigation Mode..." << endl;
        detective = new RootHunter();
    } else {
        double customPatience = InputUtils::getTolerance();
        detective = new RootHunter(customPatience);
    }
    
    DisplayUtils::printInvestigationStart();
    
    detective->huntForCrimeScene();
    
    detective->solveTheMystery();
    
    detective->presentEvidenceReport();
    
    delete detective;
    
    DisplayUtils::printGoodbye();
    
    return 0;
}
