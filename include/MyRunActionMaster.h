#ifndef MyRunActionMaster_h
#define MyRunActionMaster_h 1

#include "G4UserRunAction.hh"
#include <vector>
#include <unordered_map>
#include <G4GenericMessenger.hh>

class G4Run;
class MyConstruction;

class MyRunActionMaster : public G4UserRunAction {
public:
    MyRunActionMaster();
    virtual ~MyRunActionMaster();

    //virtual G4Run* GenerateRun();
    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
};


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
