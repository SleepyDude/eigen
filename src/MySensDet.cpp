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
//
/// @file VoxelSD.cc
/// @brief Define detector sensitivity on voxels

//#include "G4MPImanager.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"
#include "Randomize.hh"
#include "Analysis.h"
#include "MySensDet.h"
#include "G4VProcess.hh"

#include "G4SystemOfUnits.hh"

const double zEdgeDet = -69.4790179; // если пересечение со внутреннем отражателем ниже данной координаты
// то пересечение в пределах отверстия, иначе - пересечение при проходе через отражатель
const double windowS = 14.14113581; // площадь сферического сектора, см2

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
MySensDet::MySensDet(const G4String& name)
  : G4VSensitiveDetector(name)
{
  G4SDManager::GetSDMpointer()->AddNewDetector(this);
  m_testSphereCenter = {0, 0, 150*mm};
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
MySensDet::~MySensDet()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool MySensDet::ProcessHits(G4Step* astep, G4TouchableHistory*)
{
    return true;

    auto part_name = astep->GetTrack()->GetParticleDefinition()->GetParticleName();

    if (part_name == "neutron") {

        auto psp = astep->GetPreStepPoint();
        // Step принадлежит тому объему, в котором находится preStepPoint
        // Мне не важен какой объем, скорее важно то, что их наличие позволяет разделить переходы из одного в другой
        // Поэтому я просто беру значение Z для pre step point
        // Возможно было бы лучше смещать z на epsilon в сторону направления
        // и запихиваю это значение в гистограмму

        G4double E  = psp->GetKineticEnergy() / MeV;
        G4double dl = astep->GetStepLength() / cm;
//        G4double epsilonDirection = psp->GetMomentumDirection().getZ() * 1e-10;
        G4double z = psp->GetPosition().getZ() / cm;

        auto myana = Analysis::GetAnalysis();

//        myana->FillU235F("CurrentAxial", E, z, dl);

        // test part

        auto postSP = astep->GetPostStepPoint();

        G4double start = psp->GetPosition().getZ();
        G4double end = postSP->GetPosition().getZ();

//        G4cout << "step from " << start << " to " << end << " with process " << psp->GetProcessDefinedStep()->GetProcessName() << G4endl;
    }
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void MySensDet::Initialize(G4HCofThisEvent*)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void MySensDet::EndOfEvent(G4HCofThisEvent*)
{
}
