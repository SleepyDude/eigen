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
/// @file Analysis.cc
/// @brief Define histograms and ntuples

#include "G4AutoDelete.hh"
#include "G4SystemOfUnits.hh"
#include "Analysis.h"

//Select format of output here
//Note: ntuple merging is supported only with Root format
//#include "g4root.hh"

//#include "G4CsvAnalysisManager.hh"
//using G4AnalysisManager = G4CsvAnalysisManager;

#include "G4RootAnalysisManager.hh"
using G4AnalysisManager = G4RootAnalysisManager;

G4ThreadLocal G4int Analysis::fincidentFlag = false;
G4ThreadLocal Analysis* the_analysis = 0;

namespace { G4Mutex BookHistMutex = G4MUTEX_INITIALIZER; }
namespace { G4Mutex XSFileReadMutex = G4MUTEX_INITIALIZER; }
namespace { G4Mutex GammaBinsFileReadMutex = G4MUTEX_INITIALIZER; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
Analysis*
Analysis::GetAnalysis()
{
  if (!the_analysis)
    {
      the_analysis = new Analysis();
      G4AutoDelete::Register(the_analysis);
    }
  return the_analysis;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
Analysis::Analysis()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void
Analysis::Book()
{
    G4cout << "Analysis::Book start" << G4endl;
    BookNtuple();
    G4cout << "Analysis::Book finished " << G4endl;
}

void Analysis::BookNtuple() {
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
    mgr->SetNtupleMerging(true);

    m_ntuple = mgr->CreateNtuple("Universal Ntuple", "ntuple");
    mgr->CreateNtupleIColumn(m_ntuple, "type");
    mgr->CreateNtupleDColumn(m_ntuple, "xPos");
    mgr->CreateNtupleDColumn(m_ntuple, "yPos");
    mgr->CreateNtupleDColumn(m_ntuple, "zPos");
    mgr->CreateNtupleDColumn(m_ntuple, "xDir");
    mgr->CreateNtupleDColumn(m_ntuple, "yDir");
    mgr->CreateNtupleDColumn(m_ntuple, "zDir");
    mgr->CreateNtupleDColumn(m_ntuple, "len");
    mgr->CreateNtupleDColumn(m_ntuple, "E");
    mgr->FinishNtuple();

    G4cout << "Analysis::BookNtuple with " << m_ntuple << " id" << G4endl;
}

// Регистрация новой гистограммы
// `name` should be unique
void Analysis::BookHist1D(
    const G4String &name,
    const G4String &title,
    G4int nbins, 
    G4double xmin,
    G4double xmax,
    const G4String &unitName,
    const G4String &fcnName,
    const G4String &binSchemeName
) {
    G4cout << "[DEBUG] Analysis::BookHist1D()" << G4endl;
    // creating main histogram
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
    m_histHandlers[name] = mgr->CreateH1(name, title, nbins, xmin, xmax,
        unitName, fcnName, binSchemeName);
    // creating sub histogram
    My::ScaleType binScale = My::ScaleType::linSc;
    if (binSchemeName != "linear")
        binScale = My::ScaleType::logSc;
    m_supHists[name] = new My::Hist(nbins, xmin, xmax, "supHist", "supFolder", binScale);
    m_supHists[name]->setPrintFlag(false); // гистограмма нужна в рантайме
}

// Book spectrum for neutrons
void
Analysis::BookSpectrumNeutron(std::string aname) {
    BookHist1D(aname, "Neutron spectrum for " + aname, \
        500, 1.E-12*MeV, 14*MeV, "MeV", "none", "log");
}

// Book spectrum for gamma
// void
// Analysis::BookSpectrumGamma(std::string aname) {
//     BookHist1D(aname, "Gamma spectrum for " + aname, \
//         500, 1.E-12*MeV, 14*MeV, "MeV", "none", "log");
// }

// Book spectrum for gamma
void
Analysis::BookSpectrumGamma(std::string aname) {
    G4cout << "Analysis::BookSpectrum Gamma for " << aname << " starts" << G4endl;

    std::vector<double> bins = getEdges("gammaBins.txt");

    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
    int handler = mgr->CreateH1(aname, "Fluence distribution on " + aname + " detector", bins);
//    int handler = mgr->CreateH1(aname, "Fluence distribution on " + aname + " detector", \
//                                      500, 0.01*MeV, 10*MeV, "MeV", "none", "log");
    m_histHandlers[aname] = handler;

    // Установка вспомогательной гистограммы
    m_supHists[aname] = new My::Hist("gammaBins.txt", "supHist", "supFolder");
//    m_supHists[aname] = new My::Hist(500, 0.01*MeV, 10*MeV, "supHist", "supFolder", My::logSc);
    m_supHists[aname]->setPrintFlag(false);

    G4cout << "Analysis::BookSpectrum Gamma for " << aname << " finished" << G4endl;
}

void
Analysis::BookAngles(std::string aname) {

    G4cout << "Analysis::BookAngles for " << aname << " starts" << G4endl;
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
    int handler = mgr->CreateH1(aname, "Fluence distribution on " + aname + " detector", \
                                      180, 0, 180);
    m_histHandlers[aname] = handler;

    // Установка вспомогательной гистограммы
    m_supHists[aname] = new My::Hist(180, 0, 180, "supHist", "supFolder", My::linSc);
    m_supHists[aname]->setPrintFlag(false);

    G4cout << "Analysis::BookAngles for " << aname << " finished" << G4endl;
}

void Analysis::BookAxialTank1(std::string aname) {
    G4cout << "Analysis::BookAxial for " << aname << " starts" << G4endl;

    G4AnalysisManager* mgr = G4AnalysisManager::Instance();

//    std::vector<double> bins = getEdges("axialBins.txt");
//    int hdlr = mgr->CreateH1(aname, "Fluence distribution on Current Axial detector", bins);
    int hdlr = mgr->CreateH1(aname, "Fluence distribution on Current Axial detector", 60, -45, -15);
    m_histHandlers[aname] = hdlr;

    // Установка вспомогательной гистограммы
//    m_supHists[aname] = new My::Hist("axialBins.txt", "supHist", "supFolder");
    m_supHists[aname] = new My::Hist(60, -45, -15, "supHist", "supFolder");
    m_supHists[aname]->setPrintFlag(false);

    G4cout << "Analysis::BookAxial for " << aname << " finished" << G4endl;
}

void Analysis::BookAxialSilene(std::string aname) {
    G4cout << "Analysis::BookAxial for " << aname << " starts" << G4endl;

    G4AnalysisManager* mgr = G4AnalysisManager::Instance();

    std::vector<double> bins = getEdges("z_SILENE_bins.txt");
    int hdlr = mgr->CreateH1(aname, "Fluence distribution on Current Axial detector", bins);
//    int hdlr = mgr->CreateH1(aname, "Fluence distribution on Current Axial detector", 60, 100, 138.541);
    m_histHandlers[aname] = hdlr;

    // Убрал 27.05.2022 создание вспомогательной гистограммы
//    m_supHists[aname] = new My::Hist("z_SILENE_bins.txt", "supHist", "supFolder");
//    m_supHists[aname] = new My::Hist(60, 100, 138.541, "supHist", "supFolder");
//    m_supHists[aname]->setPrintFlag(false);

    G4cout << "Analysis::BookAxial for " << aname << " finished" << G4endl;
}

void Analysis::BookRadialSilene(std::string aname) {
    G4cout << "Analysis::BookRadial for " << aname << " starts" << G4endl;

    G4AnalysisManager* mgr = G4AnalysisManager::Instance();

    std::vector<double> bins = getEdges("r_SILENE_bins.txt");
    int hdlr = mgr->CreateH1(aname, "Fluence distribution on Current Axial detector", bins);
//    int hdlr = mgr->CreateH1(aname, "Fluence distribution on Current Radial detector", 60, 3.8, 18.0);
    m_histHandlers[aname] = hdlr;

    // Убрал 27.05.2022 создание вспомогательной гистограммы
//    m_supHists[aname] = new My::Hist("r_SILENE_bins.txt", "supHist", "supFolder");
//    m_supHists[aname] = new My::Hist(30, 3.8, 18.0, "supHist", "supFolder");
//    m_supHists[aname]->setPrintFlag(false);

    G4cout << "Analysis::BookRadial for " << aname << " finished" << G4endl;
}

void Analysis::BookRadialTank1(std::string aname) {
    G4cout << "Analysis::BookRadial for " << aname << " starts" << G4endl;

    G4AnalysisManager* mgr = G4AnalysisManager::Instance();

    int hdlr = mgr->CreateH1(aname, "Fluence distribution on Current Radial detector", 15, 0, 32.5);
    m_histHandlers[aname] = hdlr;

    // Установка вспомогательной гистограммы
//    m_supHists[aname] = new My::Hist("axialBins.txt", "supHist", "supFolder");
    m_supHists[aname] = new My::Hist(15, 0, 32.5, "supHist", "supFolder");
    m_supHists[aname]->setPrintFlag(false);

    G4cout << "Analysis::BookRadial for " << aname << " finished" << G4endl;
}

void
Analysis::BookSpatial(std::string aname) {
    G4cout << "Analysis::BookSpatial for " << aname << " starts" << G4endl;
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
//    int handler = mgr->CreateH3(aname, "SpatialTarget",
//                                12, //nxbins
//                                0, 6, //xmin xmax
//                                8, //nybins
//                                0, 360, //ymin ymax
//                                8, //nzbins
//                                -4, +4, //zmin, zmax
//                                "mm", //xunit
//                                "none", //yunit
//                                "mm", //zunit
//                                "none", "none", "none", // xfunc, yfunc, zfunc
//                                "linear", "linear", "linear"); // x/y/z binScheme

    int handler = mgr->CreateH3(aname, "SpatialTarget",
                                1, //nxbins
                                0, 1, //xmin xmax
                                1, //nybins
                                0, 1, //ymin ymax
                                1, //nzbins
                                0, 1, //zmin, zmax
                                "none", //xunit
                                "none", //yunit
                                "none", //zunit
                                "none", "none", "none", // xfunc, yfunc, zfunc
                                "linear", "linear", "linear"); // x/y/z binScheme
    m_histHandlers[aname] = handler;


    G4cout << "Analysis::BookSpatial for " << aname << " finished" << G4endl;
}

void Analysis::BookAxialRadial(std::string aname) {
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
    int handler = mgr->CreateH2(aname, "AxialRadial", // silene
                                60, 3.8, 18,
                                78, 100, 138.541);

//    std::vector<G4double> xbins = getEdges("r_SILENE_bins.txt");
//    std::vector<G4double> ybins = getEdges("z_SILENE_bins.txt");
//    for (auto it = xbins.begin(); it != xbins.end(); it++) {
//        G4cout << *it << G4endl;
//    }
//    for (auto it = ybins.begin(); it != ybins.end(); it++) {
//        G4cout << *it << G4endl;
//    }
//    int handler = mgr->CreateH2(aname, "AxialRadial", xbins, ybins); // silene
//    int handler = mgr->CreateH2(aname, "AxialRadial", // Tank1
//                                30, 0, 32.5,
//                                60, -45, -15);
//    int handler = mgr->CreateH2(aname, "AxialRadial", // Tank2
//                                30, 0, 53.5,
//                                60, -126.5, 126.5);
    m_histHandlers[aname] = handler;
}

void Analysis::FillAxRad(G4String aname, G4double ax, G4double ay, G4double aenergy, G4double aweight) {
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
//    G4cout << "Filling Axial Radial H2 : " << name << " (" << x << " " << y << " " << w << ")" << G4endl;
    G4double w = m_xsFisU235->GetWeight(aenergy / eV); // Микроскопическое сечение деления, барн
    G4double nu = m_nuU235->GetWeightInterpolate(aenergy / MeV); // Среднее число нейтронов деления / деление

    mgr->FillH2(m_histHandlers[aname], ax, ay, aweight * w * nu);
}

void Analysis::FillAxRadRaw(G4String aname, G4double ax, G4double ay, G4double aweight) {
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
    mgr->FillH2(m_histHandlers[aname], ax, ay, aweight);
}

void Analysis::FillRad(G4String aname, G4double ax, G4double aenergy, G4double aweight) {
    G4double w = m_xsFisU235->GetWeight(aenergy / eV); // Микроскопическое сечение деления, барн
//    G4double nu = m_nuU235->GetWeightInterpolate(aenergy / MeV); // Среднее число нейтронов деления / деление
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
    mgr->FillH1(m_histHandlers[aname], ax, aweight * w);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
Analysis::~Analysis()
{
    for (auto it = m_supHists.begin(); it != m_supHists.end(); it++) {
        delete it->second;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void
Analysis::Update()
{
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void
Analysis::Clear()
{
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void
Analysis::OpenFile(const G4String& fname)
{
  G4AnalysisManager* mgr = G4AnalysisManager::Instance();
  mgr->OpenFile(fname.c_str());
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void
Analysis::Save()
{
  G4AnalysisManager* mgr = G4AnalysisManager::Instance();
  mgr->Write();
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void
Analysis::Close(G4bool reset)
{
  G4AnalysisManager* mgr = G4AnalysisManager::Instance();
  mgr->CloseFile(reset);
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void
Analysis::FillIncident()
{
//  if (!fincidentFlag)
//    {
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
    mgr->FillH1(m_histHandlers["incident"], 0.5*cm);
//      fincidentFlag = true;
//    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void
Analysis::FillSpec(G4String name, G4double aenergy, G4double aweight)
{
    /* Заменяю эту работающую часть на вспомогательную гистограмму
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
    mgr->FillH1(m_histHandlers[name], aenergy, aweight);
    */

    // Вспомогательная гистограмма
    m_supHists[name]->fill(aenergy, aweight);
}

void
Analysis::FillSpecRaw(G4String name, G4double aenergy, G4double aweight)
{
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
    mgr->FillH1(m_histHandlers[name], aenergy, aweight);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void
Analysis::FillSpatial(G4String name, G4double x, G4double y, G4double z)
{
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
    mgr->FillH3(m_histHandlers[name], x, y, z);
}

void
Analysis::LoadToMainHist() {
    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
    for (auto it = m_supHists.begin(); it != m_supHists.end(); it++) {
        // (*it).first это строка имени, которая совпадает с именем для основных гистограмм
        std::vector<std::pair<double, double>> data = (*it).second->getDataInVec();
//        if (data.size())
//            G4cout << data.size() << G4endl;
        // Можно очистить вспомогательную гистограмму
        (*it).second->clearData();
//        // Получили вектор данных, теперь загружаем их в основную гистограмму
        for (auto it_pair = data.begin(); it_pair != data.end(); it_pair++) {
//            G4cout << "Filling " << it_pair->first << " with weight " << it_pair->second << G4endl;
            mgr->FillH1(m_histHandlers[(*it).first], it_pair->first, it_pair->second);
        }
    }
}

void Analysis::Fillntuple(G4int atype, G4ThreeVector apos, G4ThreeVector adir, G4double alen, G4double aE) {
     G4AnalysisManager* mgr = G4AnalysisManager::Instance();

     mgr->FillNtupleIColumn(m_ntuple, 0, atype);
     mgr->FillNtupleDColumn(m_ntuple, 1, apos.x());
     mgr->FillNtupleDColumn(m_ntuple, 2, apos.y());
     mgr->FillNtupleDColumn(m_ntuple, 3, apos.z());
     mgr->FillNtupleDColumn(m_ntuple, 4, adir.x());
     mgr->FillNtupleDColumn(m_ntuple, 5, adir.y());
     mgr->FillNtupleDColumn(m_ntuple, 6, adir.z());
     mgr->FillNtupleDColumn(m_ntuple, 7, alen);
     mgr->FillNtupleDColumn(m_ntuple, 8, aE);
     mgr->AddNtupleRow();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void
Analysis::FillU235F(G4String name, G4double aenergy, G4double ax, G4double aweight)
{
    // Вспомогательная гистограмма
    G4double w = m_xsFisU235->GetWeight(aenergy / eV);
//    G4cout << "Energy = " << aenergy << " and weight XS = " << w << G4endl;
    G4double nu = m_nuU235->GetWeightInterpolate(aenergy / MeV); // Ср число нейтронов деления
//    G4cout << "z = " << ax  << "   energy = " << aenergy << " nu = " << nu << G4endl;
    m_supHists[name]->fill(ax, aweight * w * nu);
//    G4AnalysisManager* mgr = G4AnalysisManager::Instance();
//    mgr->FillH1(m_histHandlers[name], ax, aweight * 1 * 1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void
Analysis::AddXSFile(G4String filename, XSFileData * xsData)
{

  G4AutoLock lock(&XSFileReadMutex);
  int res = xsData->readData(filename);

  G4cout << "File " << filename << " from thread " << G4Threading::G4GetThreadId() << " contains " << res << G4endl;
}

std::vector<double> Analysis::getEdges(std::string filename) {
    std::vector<double> bins;
    G4AutoLock lock(&GammaBinsFileReadMutex);
    std::ifstream inputFile(filename.data());
    G4String line;
    if (inputFile.is_open()) {
      double x;
      while (inputFile >> x) {
        bins.push_back(x);
      }
      inputFile.close();
    } else
      G4cout << "Unable to open file " << filename << G4endl;
    return bins;
}
