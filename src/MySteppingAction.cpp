#include "MySteppingAction.h"
#include "MyEventAction.h"
#include "MyConstruction.h"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4Neutron.hh"
#include "G4SystemOfUnits.hh"
// #include "MyHistManager.h"
//#include <mutex>
#include <cmath>
#include "G4Threading.hh"

#include "Analysis.h"
#include "G4LogicalVolumeStore.hh"

//std::mutex mymutex;
//#include <unordered_set>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MySteppingAction::MySteppingAction()
  : G4UserSteppingAction()
{
    G4cout << "In MySteppingAction constructor, thread " << G4Threading::G4GetThreadId() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MySteppingAction::~MySteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MySteppingAction::UserSteppingAction(const G4Step* step)
{
    auto part_name = step->GetTrack()->GetParticleDefinition()->GetParticleName();
    if (part_name == "gamma") {
        auto preSP = step->GetPreStepPoint();
        auto volume = preSP->GetTouchableHandle()->GetVolume();
        auto volName = volume->GetName();
        if (volName == "TankPV") { // Стенка корпуса источника
            auto myana = Analysis::GetAnalysis();

            G4ThreeVector crd = preSP->GetPosition();
            G4ThreeVector dir = preSP->GetMomentumDirection();
            G4double len = step->GetStepLength() / cm;
            G4double E = preSP->GetKineticEnergy() / MeV;
//            G4cout << "E = " << E << G4endl;
            // с записью ./eigen run1.mac  337.71s user 2.78s system 311% cpu 1:49.35 total
            // без записи ./eigen run1.mac  295.49s user 1.28s system 320% cpu 1:32.46 total
            // при включенном браузере, застрял на последних 500 ./eigen run1.mac  470.37s user 1.72s system 219% cpu 3:35.45 total  | 4124 строки
            // снова без браузера, с записью ./eigen run1.mac  419.23s user 1.47s system 337% cpu 2:04.75 total | 4184 строки
            // еще ./eigen run1.mac  300.58s user 1.33s system 327% cpu 1:32.25 total | 4191
            // без записи ./eigen run1.mac  300.27s user 1.33s system 309% cpu 1:37.53 total (чет не так) | 0 строк
            // еще раз без ./eigen run1.mac  330.57s user 1.57s system 331% cpu 1:40.33 total | 0 строк
            // с ./eigen run1.mac  305.51s user 1.38s system 308% cpu 1:39.32 total / 4210 строк
            myana->Fillntuple(1, crd, dir, len, E);
        }

    } else if (part_name == "neutron") {
        // Смотрим процесс и, если это деление, то убираем нейтроны для сохранения нормировки
        auto procDef = step->GetPostStepPoint()->GetProcessDefinedStep();
        if (procDef) {
            auto procName = procDef->GetProcessName();
            if (procName == "nFission") {
                step->GetTrack()->SetTrackStatus(fStopAndKill);
                const G4TrackVector * secondaries = step->GetSecondary();
                for (auto it = secondaries->begin(); it != secondaries->end(); it++) {
                    (*it)->SetTrackStatus(fStopAndKill);
                }
            }
        }
    } else {
        step->GetTrack()->SetTrackStatus(fStopAndKill);
        return;
    }
}
