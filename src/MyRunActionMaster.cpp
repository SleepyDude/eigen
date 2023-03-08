#include "MyRunActionMaster.h"
#include "G4Run.hh"
#include "Analysis.h"

#include <chrono>
#include <ctime>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyRunActionMaster::MyRunActionMaster()
{
    G4cout << "In MyRunActionMaster Constructor" << G4endl;
    Analysis* myana = Analysis::GetAnalysis();
    G4cout << "[DEBUG] Book analysis in MyRunActionMaster::MyRunActionMaster()" << G4endl;
    auto start_calculation = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start_calculation);
    G4cout << "[TIME START] " << std::ctime(&start_time) << G4endl;
    myana->Book();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyRunActionMaster::~MyRunActionMaster()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyRunActionMaster::BeginOfRunAction(const G4Run* /*run*/)
{
    G4cout << "[DEBUG] RunActionMaster::BeginOfRunAction" << G4endl;

    Analysis* myana = Analysis::GetAnalysis();
    myana->Clear();

    G4String fname("results-master");
    myana->OpenFile(fname);
    // OpenFile triggeres creating collecting/sending ntuples objects;
    // must be called at BeginOfRunAction
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyRunActionMaster::EndOfRunAction(const G4Run* /*run*/)
{
    G4cout << "=====================================================" << G4endl;
    G4cout << "Start EndOfRunAction for master thread"                << G4endl;
    G4cout << "=====================================================" << G4endl;

    Analysis* myana = Analysis::GetAnalysis();
    myana-> Save();
    myana-> Close();

    G4cout << "===================================================" << G4endl;
    G4cout << "End EndOfRunAction for master thread "               << G4endl;
    G4cout << "===================================================" << G4endl;
    auto end_calculation = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end_calculation);
    G4cout << "[TIME END] " << std::ctime(&end_time) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
