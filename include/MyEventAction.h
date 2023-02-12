#ifndef MyEventAction_h
#define MyEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include <unordered_map>
#include <unordered_set>

class MyRunAction;

/// Event action class
///
/// It defines data members to hold the energy deposit and track lengths
/// of charged particles in Absober and Gap layers:
/// - fEnergyAbs, fEnergyGap, fTrackLAbs, fTrackLGap
/// which are collected step by step via the functions
/// - AddAbs(), AddGap()


struct ParamsHasher
{
  std::size_t operator()(const std::pair<std::string, std::string> & ht) const
  {
    using std::size_t;
    using std::hash;
    using std::string;

    return hash<string>()(ht.first + ht.second);
  }
};

class MyEventAction : public G4UserEventAction
{
  public:
    MyEventAction();
    virtual ~MyEventAction();

    virtual void  BeginOfEventAction(const G4Event* event);
    virtual void    EndOfEventAction(const G4Event* event);

    void addEnergy(G4double e);
    void setStartEnergy(G4double e);
    G4double getEnergy();
    G4int getEventID();
    G4int getEventThread();
    void fill(std::string element, std::string param, double_t x, double_t weight, bool history=false, std::string crCatcherName="");


  private:
    static int & getCount()
    {
       static int theCount = 0;
       return theCount;
    }

    G4double     m_length;
//    G4double     m_phi;
//    G4double     m_theta;
    G4double m_lostEnergy = 0;
    G4double m_startEnergy = 0;
    G4int    m_count;
    G4int    m_eventID;
    G4int    m_circle = 10000000;
    G4int    m_nextEvent = 10000000;

    std::unordered_set< std::pair<std::string, std::string>, ParamsHasher> m_keys;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
