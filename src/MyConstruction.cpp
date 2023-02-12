#include "MyConstruction.h"

#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4IntersectionSolid.hh"
#include "G4PVReplica.hh"

//#define CUBE



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyConstruction::~MyConstruction()
{}

void MyConstruction::defineMaterials() {
#ifdef CUBE
    G4NistManager* nist = G4NistManager::Instance();
    nist->FindOrBuildMaterial("G4_Galactic");
    nist->FindOrBuildMaterial("G4_CONCRETE");
    nist->FindOrBuildMaterial("G4_Be");
    nist->FindOrBuildMaterial("G4_Al");
#endif
    G4NistManager* nist = G4NistManager::Instance();
    nist->FindOrBuildMaterial("G4_Galactic");
}

G4VPhysicalVolume* MyConstruction::defineVolumes() {
#ifndef CUBE
    m_parser.Read("model.gdml", true);
    G4VPhysicalVolume* physWorld = m_parser.GetWorldVolume(); //world volume
#else

    G4bool checkOverlaps = true;
    // WORLD
    G4double world_sizeXYZ = 500*cm;
    //Солид World
//    G4Box* solidWorld =
//        new G4Box("World",
//            0.5*world_sizeXYZ, 0.5*world_sizeXYZ, 0.5*world_sizeXYZ);
    G4Orb* solidWorld =
        new G4Orb("World",
            0.5*world_sizeXYZ);
    //Логический объем World
    G4LogicalVolume* logicWorld =
        new G4LogicalVolume(solidWorld,
                        G4Material::GetMaterial("G4_Galactic"),
                        "World");

    //Физический объем World
    G4VPhysicalVolume* physWorld =
      new G4PVPlacement(0,                     //no rotation, нет вращения
                        G4ThreeVector(),       //at (0,0,0), расположение в центре (0,0,0)
                        logicWorld,            //its logical volume, логический объем этого физического
                        "World",               //its name, название физического объема
                        0,                     //its mother  volume, материнский объем, этот самый первый
                        false,                 //no boolean operation, без логических (булевых) операций
                        0,                     //copy number, номер копии
                        checkOverlaps);        //overlaps checking, флаг проверки перекрытия объемов
    //***************************LAYER 2********************************************************************************

    //
    // Источник в виде цилиндра
    //
    G4double rIn  = 3.8*cm; // cm
    G4double rOut = 18*cm; //cm
    G4double h    = 38.541*cm;

    G4double bottomZ = 100*cm;

    // Несколько детекторов вдоль вертикальной оси

    G4int n = 6;

    G4double partWidth = h / n;
    G4double halfPartWidth = partWidth / 2.0;

    // Создание детекта/ров

    G4VSolid * srcSol = new G4Tubs("allSrcSol", rIn, rOut, h/2, 0, 360*deg);
    G4LogicalVolume * srcLV = new G4LogicalVolume(srcSol,  G4Material::GetMaterial("G4_Galactic"), "srcLV");
    new G4PVPlacement(nullptr, G4ThreeVector(0,0, bottomZ + h/2),
                      srcLV,
                      "SrcPV",
                      logicWorld,
                      false,
                      0);

    G4VSolid * onePartSolid = new G4Tubs("onePartSol", rIn, rOut, halfPartWidth, 0, 360*deg);

    G4LogicalVolume * m_detLog = new G4LogicalVolume(onePartSolid,
                                    G4Material::GetMaterial("G4_Al"),
                                   "DetPartLV");

    new G4PVReplica("DetectorReplica",
                                      m_detLog,
                                      srcLV, kZAxis, 6, partWidth);



//    for (int i = 1; i <= n; i++) {
//        G4double centerZ = bottomZ + halfPartWidth + (i-1)*partWidth;
//        G4cout << "Part " << i << " from " <<  centerZ - halfPartWidth  << " to " << centerZ + halfPartWidth << G4endl;
//        new G4PVPlacement(nullptr, G4ThreeVector(0,0, centerZ),
//                          m_detLog,
//                          "DetPart" + std::to_string(i) + "PV",
//                          logicWorld,
//                          false,
//                          i);
//    }










//    //
//    // Внутренний замедлитель
//    //
//    G4VSolid* moderatorSolid
//      = new G4Orb("InRefOrb", 74.5*mm);

//    G4LogicalVolume* modLog = new G4LogicalVolume(moderatorSolid, G4Material::GetMaterial("G4_CONCRETE"), "ModLog");
//    G4VPhysicalVolume* modPhys = new G4PVPlacement(nullptr, G4ThreeVector(0,0, 150*mm),
//                                                   modLog,
//                                                   "ModPV",
//                                                   logicWorld,
//                                                   false,
//                                                   0);

//    G4VSolid* detectorSolid
//      = new G4Orb("detOrb", 25*mm);

//    G4LogicalVolume* detLog = new G4LogicalVolume(detectorSolid, G4Material::GetMaterial("G4_Al"), "DetLog");
//    G4VPhysicalVolume* detPhys = new G4PVPlacement(nullptr, G4ThreeVector(0,0,0),
//                                                   detLog,
//                                                   "DetPV",
//                                                   modLog,
//                                                   false,
//                                                   0);

//    G4VSolid* InnerSolid
//      = new G4Orb("inerOrb", 10*mm);

//    G4LogicalVolume* inLog = new G4LogicalVolume(InnerSolid, G4Material::GetMaterial("G4_Be"), "InLog");
//    G4VPhysicalVolume* inPhys = new G4PVPlacement(nullptr, G4ThreeVector(0,0,0),
//                                                   inLog,
//                                                   "InPV",
//                                                   detLog,
//                                                   false,
//                                                   0);

//    G4VSolid* CdSolid
//      = new G4Sphere("CdSphereSolid", 74.5, 75.5, 0, 360*deg, 0, 180*deg);
//    G4VSolid* HoleSolid
//      = new G4Tubs("HoleTubeSolid", 0, 21*mm, 132*mm, 0, 360*deg);

//    G4VSolid* WindowSolid
//      = new G4IntersectionSolid("WindowSolid", CdSolid, HoleSolid, NULL, G4ThreeVector(0,0, -100*mm));

//    G4LogicalVolume* m_windowLogic = new G4LogicalVolume(WindowSolid, G4Material::GetMaterial("G4_Al"), "WindowLogic");
//    G4VPhysicalVolume* windowPhys = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 2*mm),
//                                                      m_windowLogic,
//                                                      "WindowPV",
//                                                      logicWorld,
//                                                      false,
//                                                      0);

#endif
/*
    G4LogicalVolume * sourceLV = m_parser.GetVolume("SileneSourcePV"); // Silene
//    G4LogicalVolume * sourceLV = m_parser.GetVolume("SedimentLV"); // Tank1
//    G4LogicalVolume * sourceLV = m_parser.GetVolume("UWaterLV"); // Tank2
    G4Material * mat = sourceLV->GetMaterial();
    sourceLV->SetMaterial(G4Material::GetMaterial("G4_Galactic"));

    //
    // Источник в виде цилиндра
    //

            // Silene
    G4double rIn  = 3.8*cm; // cm
    G4double rOut = 18*cm; //cm
    G4double h    = 38.541*cm;
    G4int n = 78;
    G4int nRad = 30;

            // Tank #1

//    G4double rIn  = 0*cm; // cm
//    G4double rOut = 32.5*cm; //cm
//    G4double h    = 30*cm;
//    G4int n = 60;
//    G4int nRad = 30;

        // Tank #2

//    G4double rIn  = 0*cm; // cm
//    G4double rOut = 53.5*cm; //cm
//    G4double h    = 253*cm;
//    G4int n = 60;
//    G4int nRad = 30;

//    G4double partWidth = h / n;
//    G4double halfPartWidth = partWidth / 2.0;

    // Несколько детекторов вдоль вертикальной оси





//    G4VSolid * onePartSolid = new G4Tubs("onePartSol", rIn, rOut, halfPartWidth, 0, 360*deg);

//    G4LogicalVolume * m_detLog = new G4LogicalVolume(onePartSolid,
//                                    G4Material::GetMaterial("G4_Galactic"),
//                                   "DetPartLV");

////    G4LogicalVolume * m_detLog = new G4LogicalVolume(onePartSolid,
////                                    mat,
////                                   "DetPartLV");

//    new G4PVReplica("DetectorReplica",
//                                      m_detLog,
//                                      sourceLV, kZAxis, n, partWidth);

//    // Радиальное деление



//    G4double radPartWidth = (rOut-rIn) / nRad;

//    G4VSolid * radOnePartSolid = new G4Tubs("onePartSol2", rIn, rIn+radPartWidth, halfPartWidth, 0, 360*deg);

//    G4LogicalVolume * radDetLog = new G4LogicalVolume(radOnePartSolid,
//                                    mat,
//                                   "DetPart2LV");

//    new G4PVReplica("DetectorReplica2",radDetLog,m_detLog,kRho, nRad, radPartWidth, rIn);

    // Только радиальное деление (нужно закомментить оба распределения выше)

    nRad = 60;
    G4double halfPartHeight = h / 2.0; // Половина высоты каждого цилиндрического сегмента
    G4double radPartWidth = (rOut-rIn) / nRad; // Ширина каждого сегмента

    G4VSolid * radOnePartSolid = new G4Tubs("onePartSol2", rIn, rIn+radPartWidth, halfPartHeight, 0, 360*deg);

    G4LogicalVolume * radDetLog = new G4LogicalVolume(radOnePartSolid,
                                    mat,
                                   "DetPart2LV");

    new G4PVReplica("DetectorReplica2",radDetLog, sourceLV, kRho, nRad, radPartWidth, rIn);

*/

    return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//Создаем детекторы

G4VPhysicalVolume* MyConstruction::Construct()
{  
    defineMaterials();
    return defineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
