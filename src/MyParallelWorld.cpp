//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file RE06/src/RE06ParallelWorld.cc
/// \brief Implementation of the RE06ParallelWorld class
//
// 

#include "MyParallelWorld.h"

#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "MySensDet.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4bool MyParallelWorld::fSDConstructed = false;

MyParallelWorld::MyParallelWorld(G4String worldName)
:G4VUserParallelWorld(worldName),
 fConstructed(false)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyParallelWorld::~MyParallelWorld()
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyParallelWorld::Construct()
{
  if(!fConstructed)
  { 
    fConstructed = true;
    SetupGeometry();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyParallelWorld::ConstructSD()
{
  if(!fSDConstructed)
  {
    fSDConstructed = true;
    SetupDetectors();
  }
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void MyParallelWorld::SetupGeometry()
{
  return;
  //     
  // World
  //
//  G4VPhysicalVolume* ghostWorld = GetWorld();
//  G4LogicalVolume* worldLogical = ghostWorld->GetLogicalVolume();

//  //
//  // Источник в виде цилиндра
//  //
//  G4double rIn  = 3.8*cm; // cm
//  G4double rOut = 18*cm; //cm
//  G4double h    = 38.541*cm;

//  G4double bottomZ = 100*cm;

//  // Несколько детекторов вдоль вертикальной оси

//  G4int n = 6;

//  G4double partWidth = h / n;
//  G4double halfPartWidth = partWidth / 2.0;

//  // Создание детекта/ров

//  G4VSolid * srcSol = new G4Tubs("allSrcSol", rIn, rOut, h/2, 0, 360*deg);
//  G4LogicalVolume * srcLV = new G4LogicalVolume(srcSol, nullptr, "srcLV");
//  new G4PVPlacement(nullptr, G4ThreeVector(0,0, bottomZ + h/2),
//                    srcLV,
//                    "SrcPV",
//                    worldLogical,
//                    false,
//                    0);

//  G4VSolid * onePartSolid = new G4Tubs("onePartSol", rIn, rOut, halfPartWidth, 0, 360*deg);

//  m_detLog = new G4LogicalVolume(onePartSolid,
//                                 nullptr,
//                                 "DetPartLV");

//  new G4PVReplica("DetectorReplica",
//                                    m_detLog,
//                                    srcLV, kZAxis, 6, partWidth);

//  for (int i = 1; i <= n; i++) {
//      G4double centerZ = bottomZ + halfPartWidth + (i-1)*partWidth;
////      G4cout << centerZ << G4endl;
//      G4cout << "Part " << i << " from " <<  centerZ - halfPartWidth  << " to " << centerZ + halfPartWidth << " |||| " <<  G4endl;
//      new G4PVPlacement(nullptr, G4ThreeVector(0,0, centerZ),
//                        m_detLog,
//                        "DetPart" + std::to_string(i) + "PV",
//                        worldLogical,
//                        false,
//                        i);
//  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyParallelWorld::SetupDetectors()
{
    return;
//    G4VPhysicalVolume* ghostWorld = GetWorld();
//    G4LogicalVolume* worldLogical = ghostWorld->GetLogicalVolume();

//    G4cout << "setup sens detector" << G4endl;
//    m_detLog-> SetSensitiveDetector(new MySensDet("SensSrc"));

//    m_windowLogic->SetSensitiveDetector(new MySensDet("WindowSD"));

    /* Тест который прошел
    m_testLogic->SetSensitiveDetector(new MySensDet("TestSD"));
    */
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
