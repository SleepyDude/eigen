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
/// @file Analysis.hh
/// @brief Define histograms

#ifndef ANALYSIS_MANAGER_H
#define ANALYSIS_MANAGER_H

#include "G4ThreeVector.hh"
#include <tools/histo/h1d>
#include <tools/histo/h2d>
#include <unordered_map>
#include "XSFileData.h"
#include "MyHist.h"


#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

class Analysis {
public:
  ~Analysis();

  static Analysis* GetAnalysis();

  void Book();
  void BookSpectrumNeutron(std::string aname);
  void BookSpectrumGamma(std::string aname);

  void BookAngles(std::string aname);
  void BookSpatial(std::string aname);
  void BookAxialTank1(std::string aname);
  void BookRadialTank1(std::string aname);
  void BookAxialSilene(std::string aname);
  void BookRadialSilene(std::string aname);
  void BookNtuple();
  void EndOfRun();

  void BookAxialRadial(std::string aname);
  void FillAxRad(G4String aname, G4double ax, G4double ay, G4double aenergy, G4double aweight);
  void FillAxRadRaw(G4String aname, G4double ax, G4double ay, G4double aweight);
  void FillRad(G4String aname, G4double ax, G4double aenergy, G4double aweight);

  std::vector<double> getEdges(std::string filename);

  void Update();
  void Clear();

  void OpenFile(const G4String& fname);
  void Save();
  void Close(G4bool reset = true);

  void FillIncident();
  void FillSpec(G4String name, G4double aenergy, G4double aweight);
  void FillSpecRaw(G4String name, G4double aenergy, G4double aweight);
  void LoadToMainHist();
  void FillAg109(G4String name, G4double aenergy, G4double aweight);
  void FillU235F(G4String name, G4double aenergy, G4double ax, G4double aweight);
  void FillSpatial(G4String name, G4double x, G4double y, G4double z);

  void Fillntuple(G4int atype, G4ThreeVector apos, G4ThreeVector adir, G4double alen, G4double aE);

  void AddXSFile(G4String filename, XSFileData * xsData);

  void ClearIncidentFlag();

private:
  Analysis();
  DISALLOW_COPY_AND_ASSIGN(Analysis);

  void BookHist1D(
      const G4String &name,
      const G4String &title,
      G4int nbins, 
      G4double xmin,
      G4double xmax,
      const G4String &unitName = "none",
      const G4String &fcnName = "none",
      const G4String &binSchemeName = "linear"
  );

  // Geant4 analysis manager Histograms handlers
  std::unordered_map<std::string, int> m_histHandlers;
  // Вспомогательные гистограммы
  std::unordered_map<std::string, My::Hist *> m_supHists;
  // Histogram
  // XSFileData * m_xs109;
  XSFileData * m_xsFisU235;
  XSFileData * m_nuU235;
//  G4int fincident_hist;
//  G4int ffluence_hist;
  G4int m_ntuple;
  // std::unordered_map<std::string, int> m_ntuples;

  static G4ThreadLocal G4int fincidentFlag;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
inline void Analysis::ClearIncidentFlag()
{
  fincidentFlag = false;
}

#endif

