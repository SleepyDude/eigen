#ifndef MyActionInitialization_h
#define MyActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "MyHist.h"
#include "MyHist2D.h"

class MyConstruction;

class MyActionInitialization : public G4VUserActionInitialization
{
public:
    MyActionInitialization(MyConstruction * detConstruction);
    virtual ~MyActionInitialization();
    virtual void BuildForMaster() const;//Создание источника начальных источников частиц
    virtual void Build() const;//Создание источника начальных источников частиц
private:
    MyConstruction * m_detConstruction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
