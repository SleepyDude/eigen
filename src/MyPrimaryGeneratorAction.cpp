#include "MyPrimaryGeneratorAction.h"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <cmath>
#include "MyHistManager.h"
#include "Utils.h"

#include "G4AutoDelete.hh"
#include "Analysis.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

namespace { G4Mutex ReadSpatialMutex = G4MUTEX_INITIALIZER; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Класс, в котором описывается положение, тип, енергия частиц, направление вылета
// и распределенние начальных частиц
MyPrimaryGeneratorAction::MyPrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(),
      m_particleGun(nullptr)
{
    // По умолчанию поставим 1 частицу
    G4int n_particle = 1;
    m_particleGun  = new G4ParticleGun(n_particle);

    // default particle kinematic
    // Получаем встроеную в Geant4 таблицу частиц
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName;
    // Ищем частицу, в нашем случае нейтрон
    G4ParticleDefinition* particle
        = particleTable->FindParticle(particleName="neutron");
//    G4ParticleDefinition* particle
//        = particleTable->FindParticle(particleName="gamma");
    // Устанавливаем полученную частицу как испускаемые начальные частицы в источнике
    m_particleGun->SetParticleDefinition(particle);

//    G4cout << "Energy: " << m_particleGun->GetParticleEnergy() << G4endl;


    G4double startEnergy = 2*MeV;
    m_a = m_a0 + m_a1 * startEnergy + m_a2 * startEnergy * startEnergy;

//  Розыгрыш энергии для первого ивента в ране из формулы Ватта для потери связи с 2 МэВ
//    for (int i = 0; i < 10; i++) {
//        G4double K = 1. + 1./(8 * m_a);
//        G4double L = (K + sqrt(K*K - 1))/m_a;
//        G4double M = m_a * L - 1;
//        G4double x;
//        G4double y;
//        do {
//            x = -log(G4UniformRand());
//            y = -log(G4UniformRand());
//        } while ((y - M*(x+1))*(y - M*(x+1)) > L * x);
//        startEnergy = L * x;
//        m_a = m_a0 + m_a1 * startEnergy + m_a2 * startEnergy * startEnergy;
//    }

    fillAxialRadial("Tank1-april", 60);
//    fillAxialRadial("Tank2-april", 60);
//    fillAxialRadial("Tank1noWater-april", 60);
//    fillAxialRadial("Silene-april", 60);
//    fillAxialRadial("Silene-april", 1);

//    G4AutoLock lock(&ReadSpatialMutex);
//    for (size_t i = 0; i < m_binsAxial.size(); i++) {
//        G4cout << m_binsAxial[i] << " " << m_dataAxial[i] << G4endl;
//    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Деструктор, удаляем созданный в конструкторе экземпляр класса источника G4ParticleGun
MyPrimaryGeneratorAction::~MyPrimaryGeneratorAction()
{
    delete m_particleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    /* test

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle
        = particleTable->FindParticle("geantino");
    m_particleGun->SetParticleDefinition(particle);
    m_particleGun->SetParticlePosition(G4ThreeVector(50*cm, 0*cm, 120*cm));
    m_particleGun->SetParticleMomentumDirection(G4ThreeVector(-1, 0, 0));

    m_particleGun->GeneratePrimaryVertex(anEvent);
    return; */ // end test

    // Розыгрыш координаты источника

    G4double axial;
    G4double radial;
    G4double phi_crd = 2 * M_PI * G4UniformRand();

    // Равномерное распределение или из файла
    if (!m_isUniform) {
//        G4cout << "Non Uniform " << G4endl;
        axial = lookForX(m_binsAxial, m_dataAxial);

//        G4cout << "axial =" << axial << G4endl;

        // ищу нужный индекс радиуса

        int radIndex = static_cast<int>((axial - *m_binsAxial.begin()) / m_axWidth);

//        G4cout << "index is" << radIndex << G4endl;

        radial = lookForX(m_binsRadial[radIndex], m_dataRadial[radIndex])*cm;

//        G4cout << "radial is" << radial << G4endl;

        axial *= cm;
//        G4cout << axial << " " << radial << G4endl;
    } else {
//        G4cout << "Uniform " << G4endl;
//         Tank 1

//        G4double R2 = 22.5;
//        G4double h  = 18.21;
//        G4double zmin = - 30;


        G4double R2 = 32.5;
        G4double h  = 30;
        G4double zmin = -45;

        axial  = (zmin + h*G4UniformRand() ) * cm;
//        G4cout << axial << G4endl;
        radial = sqrt(R2*R2*G4UniformRand()) * cm;
    }

    G4double x_crd = radial * cos(phi_crd);
    G4double y_crd = radial * sin(phi_crd);
    G4double z_crd = axial;

//    G4cout << "Start Position " << G4endl;
//    G4cout << "[ " << x_crd << " " << y_crd << " " << z_crd << " ]" << G4endl;

    m_particleGun->SetParticlePosition(G4ThreeVector(x_crd, y_crd, z_crd));

    // Розыгрыш направления
    // Изотропно
    G4double phi = 2 * M_PI * G4UniformRand();
    G4double cosTheta = 2 * G4UniformRand() - 1.;
    G4double sinTheta = sqrt(1 - cosTheta * cosTheta);
    G4double xCos = sinTheta * cos(phi);
    G4double yCos = sinTheta * sin(phi);
    G4double zCos = cosTheta;
    m_particleGun->SetParticleMomentumDirection(G4ThreeVector(xCos, yCos, zCos));

    // Розыгрыш энергии из формулы Ватта.
    // Основывается на m_a с предыдущей итерации и вычисляется m_a для следующей

    G4double K = 1. + 1./(8 * m_a);
    G4double L = (K + sqrt(K*K - 1))/m_a;
    G4double M = m_a * L - 1;
    G4double x;
    G4double y;
    do {
        x = -log(G4UniformRand());
        y = -log(G4UniformRand());
    } while ((y - M*(x+1))*(y - M*(x+1)) > L * x);
    G4double energy = L * x;

    // Считаю m_a для следующей за этой частицы
    m_a = m_a0 + m_a1 * energy + m_a2 * energy * energy;
    m_particleGun->SetParticleEnergy(energy);

    // Записываем нейтроны источника с целью дебага в нтупл. Вроде все 10000 историй нормально записались
//    auto myana = Analysis::GetAnalysis();
//    myana->Fillntuple(G4ThreeVector(x_crd, y_crd, z_crd), G4ThreeVector(xCos, yCos, zCos), 1.0, energy / MeV);

    // Создаем начальное событие, запускаем первичную частицу
    m_particleGun->GeneratePrimaryVertex(anEvent);
}

//void MyPrimaryGeneratorAction::createMessenger() {
//    m_messenger = new G4GenericMessenger(this, "/primary/");
//    m_messenger->DeclarePropertyWithUnit("setSrcRad", "cm", m_srcRad);
//    m_messenger->DeclarePropertyWithUnit("setSrcL", "cm", m_srcL);
//    m_messenger->DeclareProperty("setSigma", m_sigma);
//}

//const double epsilon = 1E-10;

void MyPrimaryGeneratorAction::fillAxialRadial(G4String folder, int n) {
    G4AutoLock lock(&ReadSpatialMutex);

    std::string AxialFilename = folder + "/current/Axial.txt";
    std::ifstream inputFileStream1(AxialFilename.c_str());
    if (inputFileStream1.fail()) {
        G4cout << "Uniform mode, can't open " << AxialFilename.c_str() << G4endl;
        m_isUniform = true; // Не меняется динамически, нужна для того чтобы не менять розыгрыш в функции
        return;
    } else {
        G4cout << "Axial distribution from " << AxialFilename.c_str() << G4endl;
    }



    for (std::string line; std::getline(inputFileStream1, line);) {
//        std::vector<std::string> tokens = ::split(line, ' ');
//        G4cout << line << G4endl;
        if (line[0] != '#') {
            std::vector<std::string> tokens;
            ::tokenize(line, tokens, " ", true);
//            G4cout << std::stod(tokens[0]) << " " << std::stod(tokens[1]) << G4endl;
            if (tokens.size() == 1) { // Возможно, что разделителем служит таб
                tokens.clear();
                ::tokenize(line, tokens, "\t", true);
            }
            try {
                m_binsAxial.push_back(std::stod(tokens[0]));
                m_dataAxial.push_back(std::stod(tokens[1]));
    //            G4cout << std::stod(tokens[0]) << " " << std::stod(tokens[1]) << G4endl;
                } catch (const std::exception& e) {
                    G4cerr << e.what() << G4endl;
            }
        }
    }

    // На данном этапе имеется заполненный аксиальный массив
    // причем имеются нули в начале и в конце поэтому устанавливаю правило по которому в файле в конце не должно быть нуля и в начале только один ноль


    G4double fullWidth = *std::prev(m_binsAxial.end()) - *m_binsAxial.begin();
    // Записываю границы
    m_axWidth = fullWidth / (double)n; // epsilon
//    m_axWidth = 30.0/n;

//    for (int i = 0; i <= n; i++) { // для n=1 в m_clusterAxials будет 2 элемента - начало и конец диапазона
//        m_clusterAxials.push_back(*m_binsAxial.begin() + axWidth * i);
//    }

    for (int i = 0; i < n; i++) {
//        std::string RadialFilename = std::to_string(n) + "/PrevRadial (" + std::to_string(i+1) + ").txt";
        std::string RadialFilename = folder + "/current/Radial " + std::to_string(i+1) + ".txt";

        std::ifstream inputFileStream2(RadialFilename.c_str());
        if (inputFileStream2.fail()) {
            G4cout << "Uniform mode, can't open " << RadialFilename.c_str() << G4endl;
            m_isUniform = true;
            return;
        } else {
            G4cout << "Radial distribution from " << RadialFilename.c_str() << G4endl;
        }

        m_binsRadial.push_back(std::vector<double>());
        m_dataRadial.push_back(std::vector<double>());

        G4cout << "At " << i << "th file" << G4endl;

        for (std::string line; std::getline(inputFileStream2, line);) {
//            G4cout << line << G4endl;
            if (line[0] != '#') {
                std::vector<std::string> tokens;
                ::tokenize(line, tokens, " ", true);
//                G4cout << std::stod(tokens[0]) << " " << std::stod(tokens[1]) << G4endl;
                if (tokens.size() == 1) { // Возможно, что разделителем служит таб
                    tokens.clear();
                    ::tokenize(line, tokens, "\t", true);
                }
                try {
//                    G4cout << std::stod(tokens[0]) << " " << std::stod(tokens[1]) << G4endl;
//                    G4cout << tokens.size() << G4endl;
                    m_binsRadial[i].push_back(std::stod(tokens[0]));
                    m_dataRadial[i].push_back(std::stod(tokens[1]));
                    } catch (const std::exception& e) {
                        G4cerr << e.what() << G4endl;
                }
            }
        }

//        G4cout << "Filled " << i << "th file" << G4endl;
    }

    // make data part sum
    for (auto it = (m_dataAxial.begin()+1); it != m_dataAxial.end(); it++) {
        (*it) += *std::prev(it);
    }

    for (int i = 0; i < n; i++) {
        for (auto it = (m_dataRadial[i].begin()+1); it != m_dataRadial[i].end(); it++) {
    //        G4cout << "radialData " << (*it) << G4endl;
            (*it) += *std::prev(it);
    //        G4cout << "radialData2 " << (*it) << G4endl;
        }
    }

    // Нормировка частичных сумм
    double maxSum = *std::prev(m_dataAxial.end());
    for (auto it = (m_dataAxial.begin()+1); it != m_dataAxial.end(); it++) {
        (*it) /= maxSum;
    }

    for (int i = 0; i < n; i++) {
        maxSum = *std::prev(m_dataRadial[i].end());
        for (auto it = (m_dataRadial[i].begin()+1); it != m_dataRadial[i].end(); it++) {
            (*it) /= maxSum;
        }
    }

    G4cout << "Finish filling arrays from files " << G4endl;
    m_isUniform = false;
}

double MyPrimaryGeneratorAction::lookForX(std::vector<double> const & xvec, std::vector<double> const & yvec) {

//    G4cout << "looking for X" << G4endl;

    G4double ksi1 = G4UniformRand();
    // Binary search coefficient in func array
    size_t left  = 0;
    size_t right = yvec.size() - 1;
    while(right - left != 1) {
        size_t mid = (left + right) / 2;
        if (yvec[mid] < ksi1)
            left = mid;
        else
            right = mid;
    }
//    G4cout << "[" << yvec[left] << "] " << ksi1 << " [" << yvec[right] << "]" << G4endl;
//    G4cout << "[" << left << "] " << " [" << right << "]" << G4endl;
//    double enMin = xvec[left];
//    double enMax = xvec[right];
    // interpolation
//    double x = (xvec[right] + xvec[left]) / 2;
    double_t k = (xvec[right] - xvec[left]) / (yvec[right] - yvec[left]);
    double x = k * (ksi1 - yvec[left]) + xvec[left];
//    G4cout << "x = " << x << G4endl;

    return x;
}
