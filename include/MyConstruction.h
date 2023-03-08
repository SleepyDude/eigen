#ifndef MyConstruction_h
#define MyConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
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
    virtual G4VPhysicalVolume* Construct() override;

private:
    G4VPhysicalVolume* defineVolumes();
    G4GDMLParser m_parser;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
