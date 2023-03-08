#include "MyConstruction.h"
#include "StateManager.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyConstruction::~MyConstruction()
{}

G4VPhysicalVolume* MyConstruction::defineVolumes() {
    // m_parser.SetVerboseLevel(0); // TODO: fix possible GDML validation errors
    auto sm = StateManager::GetStateManager();
    std::string modelfn = sm->getFilename("model");
    m_parser.Read(modelfn, true);
    G4VPhysicalVolume* physWorld = m_parser.GetWorldVolume();
    return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//Создаем детекторы

G4VPhysicalVolume* MyConstruction::Construct()
{
    return defineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
