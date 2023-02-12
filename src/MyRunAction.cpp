#include "MyRunAction.h"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//#include "MyHistManager.h"
#include "Analysis.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
MyRunAction::MyRunAction()
{
    // Book analysis in ctor
    Analysis* myana = Analysis::GetAnalysis();
    G4cout<<"Book analysis " << G4endl;
    myana->Book();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
MyRunAction::~MyRunAction()
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void MyRunAction::BeginOfRunAction(const G4Run* /*run*/)
{
    G4cout << "RunAction::BeginOfRunAction" << G4endl;

    Analysis* myana = Analysis::GetAnalysis();

    std::ostringstream fname;
    fname<<"result"<<"-thread"<<G4Threading::G4GetThreadId();
    myana->OpenFile(fname.str());
    // OpenFile triggeres creating collecting/sending ntuples objects;
    // must be called at BeginOfRunAction
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void MyRunAction::EndOfRunAction(const G4Run*)
{
    //NOTE: if only histograms are active actually we do not create the per-thread
    //ntuple file

    Analysis* myana = Analysis::GetAnalysis();
    myana->Save();
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
