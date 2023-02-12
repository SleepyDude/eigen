#include "MyEventAction.h"
#include "MyRunAction.h"
#include "MyHist.h"

#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

#include "G4SystemOfUnits.hh"

#include "MyHistManager.h"

#include "Analysis.h"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyEventAction::MyEventAction()
 : G4UserEventAction()
{
    getCount()++;
    m_count = getCount();
    G4cout << "Event action creating " << m_count << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyEventAction::~MyEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyEventAction::BeginOfEventAction(const G4Event* event/*event*/)
{
    m_eventID = event->GetEventID();
//    if (m_eventID >= m_nextEvent) {
//        m_nextEvent += m_circle;
//        My::HistManager::getInstance().dumpHistograms(m_eventID);
//        G4cout << "Next Event = " << m_nextEvent << G4endl;
//        G4cout << "EventID = " << m_eventID << G4endl;
//    }
//        G4cout << "Event ID = " << m_eventID << G4endl;
    Analysis * myana = Analysis::GetAnalysis();
    myana->LoadToMainHist();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyEventAction::EndOfEventAction(const G4Event* event)
{
//    G4cout << "============================" << G4endl;
    for (auto item : m_keys) {
//        G4cout << item.first << " | " << item.second << G4endl;

        My::HistManager::getInstance().sqrData(std::to_string(m_count) + item.first, item.second);
        My::HistManager::getInstance().fill(item.first, item.second, std::to_string(m_count)+item.first, item.second);
        My::HistManager::getInstance().clearData(std::to_string(m_count)+item.first, item.second);
        My::HistManager::getInstance().setPrintFlag(std::to_string(m_count)+item.first, item.second, false);
    }

    // В конце эвента загружаю данные временных гистограмм в основные
    // 27.11.2021
//    Analysis * myana = Analysis::GetAnalysis();
//    myana->LoadToMainHist();

//    G4cout << "============================" << G4endl;
}

void MyEventAction::addEnergy(G4double e) {
    m_lostEnergy -= e;
//    G4cout << "Add energy, now total " << m_lostEnergy << G4endl;
}

void MyEventAction::setStartEnergy(G4double e) {
    m_startEnergy = e;
}

G4double MyEventAction::getEnergy() {
    return m_startEnergy;
//    m_lostEnergy = 0;
}

G4int MyEventAction::getEventID() {
    return m_eventID;
}

G4int MyEventAction::getEventThread() {
    return m_count;
}

void MyEventAction::fill(std::string element, std::string param, double_t x, double_t weight, bool history, std::string crCatcherName) {
    if (!history) {
        My::HistManager::getInstance().fill(element, param, x, weight, crCatcherName);
    } else {
//        G4cout << "Inserting " << element+crCatcherName << G4endl;
        m_keys.insert(std::make_pair(element+crCatcherName, param));
        My::HistManager::getInstance().fill(std::to_string(m_count) + element, param, x, weight, crCatcherName);
    }
}
