#ifndef MyActionInitialization_h
#define MyActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class MyActionInitialization : public G4VUserActionInitialization
{
public:
    MyActionInitialization();
    virtual ~MyActionInitialization();
    virtual void BuildForMaster() const;//Создание источника начальных источников частиц
    virtual void Build() const;//Создание источника начальных источников частиц
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
