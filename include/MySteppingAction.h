#ifndef MySteppingAction_h
#define MySteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4LogicalVolume.hh"

class MyConstruction;
class MyEventAction;

/// Stepping action class.
///
/// In UserSteppingAction() there are collected the energy deposit and track
/// lengths of charged particles in Absober and Gap layers and
/// updated in B4aEventAction.

class MySteppingAction : public G4UserSteppingAction
{
public:
  MySteppingAction();
  virtual ~MySteppingAction();

  virtual void UserSteppingAction(const G4Step* step);

private:
  G4LogicalVolume * m_detector;
  G4double m_srcOffset; // Наименьшее значение z для цилиндра с делящимся веществом // cm
  G4double m_srcHeight; // cm
  int      m_srcBins;
  G4double m_binWidth;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
