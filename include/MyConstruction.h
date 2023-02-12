#ifndef MyConstruction_h
#define MyConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include <vector>

#include "G4GDMLParser.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;

/// Detector construction class to define materials and geometry.
/// Класс создания геомертии детектора и задания материалов

class MyConstruction : public G4VUserDetectorConstruction
{
public:
    MyConstruction() = default;
    virtual ~MyConstruction() override;
    //Объявление и создание детекторов и среды
    virtual G4VPhysicalVolume* Construct() override;
    const G4VPhysicalVolume* getDetectorPV(size_t layerNum) const;


private:
    void defineMaterials();
    G4VPhysicalVolume* defineVolumes();

    std::vector<G4VPhysicalVolume *> m_detectorsPV;
    G4GDMLParser m_parser;

};

inline const G4VPhysicalVolume* MyConstruction::getDetectorPV(size_t layerNum) const {
    if (!m_detectorsPV.empty())
        return m_detectorsPV[layerNum];
    return nullptr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
