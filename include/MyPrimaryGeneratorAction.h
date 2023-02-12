#ifndef MyPrimaryGeneratorAction_h
#define MyPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include <G4GenericMessenger.hh>
#include "MyEventAction.h"

class G4ParticleGun;
class G4Event;
class G4Box;

class MyPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    MyPrimaryGeneratorAction();
    virtual ~MyPrimaryGeneratorAction();

    // method from the base class
    // Метод из базового класса, задание источника начальных частиц
    virtual void GeneratePrimaries(G4Event*);         
  
    // method to access particle gun
    // Метод для доступа к истчнику частиц (пушке частиц ;) )
    const G4ParticleGun* getParticleGun() const { return m_particleGun; }

//    void createMessenger();
  
private:
    void fillAxialRadial(G4String folder, int n=1);
    double lookForX(const std::vector<double> &xvec, const std::vector<double> &yvec);


    G4ParticleGun *  m_particleGun;

    G4GenericMessenger * m_messenger;

    // Коэффициенты для урана-235 из работы UCRL-AR-228518
    //Simulation of Neutron and Gamma Ray Emission from Fission
    //J. Verbeke, C. Hagmann, D. M. Wright
    //March 1, 2007
    G4double m_a;
    G4double m_a0 = 0.920108;
    G4double m_a1 = -0.00936909;
    G4double m_a2 = 7.32627e-05;

    std::vector<double> m_binsAxial;
    std::vector<double> m_dataAxial;
    std::vector< std::vector<double> > m_binsRadial;
    std::vector< std::vector<double> > m_dataRadial;

//    std::vector<double> m_clusterAxials;
    G4double m_axWidth;

    bool m_isUniform = true;

//    std::string m_axialFilename = "PrimAxial.txt";
//    std::string m_radialFilename = "PrimRadial.txt";

};
#endif
