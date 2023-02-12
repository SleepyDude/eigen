#include "MyRunActionMaster.h"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"

#include "Analysis.h"

// #include "MyHistManager.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyRunActionMaster::MyRunActionMaster()
{
    // Book analysis in ctor
    Analysis* myana = Analysis::GetAnalysis();
    G4cout<<"Book analysis " << G4endl;
    myana->Book();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyRunActionMaster::~MyRunActionMaster()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyRunActionMaster::BeginOfRunAction(const G4Run* /*run*/)
{
    G4cout << "RunActionMaster::BeginOfRunAction" << G4endl;

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
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
