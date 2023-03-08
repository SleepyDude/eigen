#ifndef MyHistManager_h
#define MyHistManager_h 1

#include "MyHist.h"
#include "MyHist2D.h"

#include <unordered_map>
#include <map>
#include <cstring>
#include <utility>
#include <vector>
#include <iostream>
#include <fstream>
#include "safe_ptr.h"

#include <G4GenericMessenger.hh>
#include "CrossCatcher.h"

#include <atomic>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   //define something for Windows (32-bit and 64-bit, this part is common)
    #include <conio.h>
    #include <direct.h>
    #include <process.h>
    #include <stdio.h>
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__
    #include <sys/stat.h>
    #include <sys/types.h>
#elif __ANDROID__
    // Below __linux__ check should be enough to handle Android,
    // but something may be unique to Android.
#elif __linux__
    // linux
    #include <bits/stdc++.h>
    #include <sys/stat.h>
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif

namespace My
{

class HistManager {

struct HistParams {
    size_t size;
    double_t binMin;
    double_t binMax;
    My::ScaleType scale = linSc;
    std::string scaleFile = "bins.txt";
} typedef HistParams;

struct HistTuple {
    std::string element;
    std::string param;
    std::string resultFolder;

    HistTuple(std::string e, std::string p, std::string r)
        : element(e), param(p), resultFolder(r)
    {}

    bool operator==(const HistTuple &other) const
    { return (element == other.element
              && param == other.param
              && resultFolder == other.resultFolder);
    }
} typedef HistTuple;

struct HistTupleHasher
{
  std::size_t operator()(const HistTuple& ht) const
  {
    using std::size_t;
    using std::hash;
    using std::string;

    return hash<string>()(ht.element + ht.param + ht.resultFolder);
  }
};


public:

    static HistManager& getInstance() {
        static HistManager instance;
//        std::cout << "Instance" << std::endl;
        return instance;
    }

    void createResFolder() {
        std::cout << "Creating folder " << m_resultFolder << " from MyHistManager" << std::endl;
        if (!m_resultFolder.empty()) {
#if defined(_WIN32)
            int errcode = _mkdir(m_resultFolder.c_str());
#else
            int errcode = mkdir(m_resultFolder.c_str(), 755);
#endif
            if(!(!errcode || errno == 17)) { // 17 - folder already exists, all right
                std::cerr << "Error :  " << strerror(errno) << std::endl;
            }
        }
    }

    void addParams(std::string name, HistParams params) {
//        std::cerr << "scale type = " << params.scale << std::endl;
        (*m_initData)[name] = std::vector<HistParams>{params};
//        std::cerr << "INSIDE ADDPARAMS, scale type is" << params.scale << std::endl;
    }

    void addParams(std::string name, HistParams paramsX, HistParams paramsY) {
        (*m_initData)[name] = std::vector<HistParams>{paramsX, paramsY};
    }

    void addParams1D(std::string name, size_t size, double_t binMin, double_t binMax) {
        (*m_initData)[name] = std::vector<HistParams>{{size, binMin, binMax}};
    }

    void addParams2D(std::string name, size_t sizeX, double_t binMinX, double_t binMaxX, size_t sizeY, double_t binMinY, double_t binMaxY) {
        (*m_initData)[name] = std::vector<HistParams>{{sizeX, binMinX, binMaxX}, {sizeY, binMinY, binMaxY}};
    }

    My::Hist* getHist1D(std::string element, std::string param) {
        if (m_hists1D->find(HistTuple(element, param, m_resultFolder)) == m_hists1D->end()) {
            if (m_initData->find(param) == m_initData->end()) {
                std::cerr << "No such param data (" << param << "). Use 'addParams' methord previously.";
                return nullptr;
            }
            std::vector<HistParams> hp = (*m_initData)[param];
            (*m_hists1D)[HistTuple(element, param, m_resultFolder)] = new My::Hist(hp[0].size, hp[0].binMin, hp[0].binMax, param, m_resultFolder + "\\" + element);
        }
        return (*m_hists1D)[HistTuple(element, param, m_resultFolder)];
    }

    My::Hist2D* getHist2D(std::string element, std::string param) {
    //    std::cout << "get hist with params " << element << " " << param << std::endl;
        if (m_hists2D->find(HistTuple(element, param, m_resultFolder)) == m_hists2D->end()) {
    //        std::cout << "get hist with params" << element << " " << param << std::endl;
            if (m_initData->find(param) == m_initData->end()) {
                std::cerr << "No such param data (" << param << "). Use 'addParams' methord previously.";
                return nullptr;
            }
            std::vector<HistParams> hp = (*m_initData)[param];
            (*m_hists2D)[HistTuple(element, param, m_resultFolder)] = new My::Hist2D(hp[0].size, hp[0].binMin, hp[0].binMax, \
                    hp[1].size, hp[1].binMin, hp[1].binMax, param, m_resultFolder + "\\" + element);
    //        std::cout << m_hists2D[std::make_pair(element, param)] << std::endl;
        }
        return (*m_hists2D)[HistTuple(element, param, m_resultFolder)];
    }

    std::vector<My::Hist*> getHists1(std::string param)  {
        std::vector<My::Hist*> hists;
        for (auto it = m_hists1D->begin(); it != m_hists1D->end(); it ++) {
            if (it->first.param == param) {
                hists.push_back(it->second);
            }
        }
        return hists;
    }

    void fillLET(std::string element, double weight) {
        if (m_LETmap->find(element) == m_LETmap->end()) {
            (*m_LETmap)[element] = 0.0;
        }
        (*m_LETmap)[element] = (*m_LETmap)[element] + weight;
        static int counter = 0;
        if (counter % 100000 == 0)
            std::cout << "LET[" << element << "] = " << (*m_LETmap)[element] << std::endl;
        counter++;
    }

    void fill(std::string element, std::string param, double_t x, double_t weight, std::string crCatcherName="") {
//        G4cout << "INSIDE FILL" << std::endl;
        if (m_hists1D->find(HistTuple(element + crCatcherName, param, m_resultFolder)) == m_hists1D->end()) {
            if (m_initData->find(param) == m_initData->end()) {
                G4cout << "No such param data (" << param << "). Use 'addParams' methord previously.";
            }
            std::vector<HistParams> hp = (*m_initData)[param];
//            std::cout << "scale param in fill in MyHistManager is " << hp[0].scale << std::endl;
            (*m_hists1D)[HistTuple(element + crCatcherName, param, m_resultFolder)] = new My::Hist(hp[0].size, hp[0].binMin, hp[0].binMax, param, m_resultFolder + "\\" + element + crCatcherName, hp[0].scale, hp[0].scaleFile);
//            G4cout << *((*m_hists1D)[std::make_pair(element, param)]) << G4endl;
        }

        double_t coef = 1.;
        if (!crCatcherName.empty()) {
            if (m_crossCatchers->find(crCatcherName) == m_crossCatchers->end()) {
                (*m_crossCatchers)[crCatcherName] = new My::CrossCatcher(crCatcherName + ".txt");
            }
            coef = (*m_crossCatchers)[crCatcherName]->getCoef(x*1000000.0);
        }
//        G4cout << "weight in HM = " << coef * weight << std::endl;
        (*m_hists1D)[HistTuple(element + crCatcherName, param, m_resultFolder)]->fill(x, weight * coef);
        // КОСТЫЛЬ
        (*m_hists1D)[HistTuple(element + crCatcherName, param, m_resultFolder)]->fill(1000000, weight * coef);
//        inc(element+param);
    }

    void fill(std::string element, std::string param, std::string element2, std::string param2) {
//        G4cout << "INSIDE FILL" << std::endl;
        if (m_hists1D->find(HistTuple(element, param, m_resultFolder)) == m_hists1D->end()) {
            if (m_initData->find(param) == m_initData->end()) {
                G4cout << "No such param data (" << param << "). Use 'addParams' methord previously.";
            }
            std::vector<HistParams> hp = (*m_initData)[param];
//            std::cout << "scale param in fill in MyHistManager is " << hp[0].scale << std::endl;
            (*m_hists1D)[HistTuple(element, param, m_resultFolder)] = new My::Hist(hp[0].size, hp[0].binMin, hp[0].binMax, param, m_resultFolder + "\\" + element, hp[0].scale, hp[0].scaleFile);
//            G4cout << *((*m_hists1D)[std::make_pair(element, param)]) << G4endl;
        }

        (*m_hists1D)[HistTuple(element, param, m_resultFolder)]->fill( *(*m_hists1D)[HistTuple(element2, param2, m_resultFolder)] );
//        inc(element+param);
    }

    void clearData(std::string element, std::string param) {
//        G4cout << "INSIDE FILL" << std::endl;
        if (m_hists1D->find(HistTuple(element, param, m_resultFolder)) == m_hists1D->end()) {
            if (m_initData->find(param) == m_initData->end()) {
                G4cout << "No such param data (" << param << "). Use 'addParams' methord previously.";
            }
            std::vector<HistParams> hp = (*m_initData)[param];
            (*m_hists1D)[HistTuple(element, param, m_resultFolder)] = new My::Hist(hp[0].size, hp[0].binMin, hp[0].binMax, param, m_resultFolder + "\\" + element, hp[0].scale, hp[0].scaleFile);
        }

        (*m_hists1D)[HistTuple(element, param, m_resultFolder)]->clearData();
    }

    void sqrData(std::string element, std::string param) {
        if (m_hists1D->find(HistTuple(element, param, m_resultFolder)) == m_hists1D->end()) {
            if (m_initData->find(param) == m_initData->end()) {
                G4cout << "No such param data (" << param << "). Use 'addParams' methord previously.";
            }
            std::vector<HistParams> hp = (*m_initData)[param];
            (*m_hists1D)[HistTuple(element, param, m_resultFolder)] = new My::Hist(hp[0].size, hp[0].binMin, hp[0].binMax, param, m_resultFolder + "\\" + element, hp[0].scale, hp[0].scaleFile);
        }

        (*m_hists1D)[HistTuple(element, param, m_resultFolder)]->sqrData();
    }

    void setPrintFlag(std::string element, std::string param, bool flag) {
        if (m_hists1D->find(HistTuple(element, param, m_resultFolder)) == m_hists1D->end()) {
            if (m_initData->find(param) == m_initData->end()) {
                G4cout << "No such param data (" << param << "). Use 'addParams' methord previously.";
            }
            std::vector<HistParams> hp = (*m_initData)[param];
            (*m_hists1D)[HistTuple(element, param, m_resultFolder)] = new My::Hist(hp[0].size, hp[0].binMin, hp[0].binMax, param, m_resultFolder + "\\" + element, hp[0].scale, hp[0].scaleFile);
        }

        (*m_hists1D)[HistTuple(element, param, m_resultFolder)]->setPrintFlag(flag);
    }

    void setDivType(std::string element, std::string param, My::DivideStatus divStatus) {
        if (m_hists1D->find(HistTuple(element, param, m_resultFolder)) == m_hists1D->end()) {
            if (m_initData->find(param) == m_initData->end()) {
                G4cout << "No such param data (" << param << "). Use 'addParams' methord previously.";
            }
            std::vector<HistParams> hp = (*m_initData)[param];
            (*m_hists1D)[HistTuple(element, param, m_resultFolder)] = new My::Hist(hp[0].size, hp[0].binMin, hp[0].binMax, param, m_resultFolder + "\\" + element, hp[0].scale, hp[0].scaleFile);
        }
        (*m_hists1D)[HistTuple(element, param, m_resultFolder)]->setDivStatus(divStatus);
    }

    void setLazyScale(std::string element, std::string param, double lazyScale) {
        if (m_hists1D->find(HistTuple(element, param, m_resultFolder)) == m_hists1D->end()) {
            if (m_initData->find(param) == m_initData->end()) {
                G4cout << "No such param data (" << param << "). Use 'addParams' methord previously.";
            }
            std::vector<HistParams> hp = (*m_initData)[param];
            (*m_hists1D)[HistTuple(element, param, m_resultFolder)] = new My::Hist(hp[0].size, hp[0].binMin, hp[0].binMax, param, m_resultFolder + "\\" + element, hp[0].scale, hp[0].scaleFile);
        }
        (*m_hists1D)[HistTuple(element, param, m_resultFolder)]->setScaleData(lazyScale);
    }

    void setCrossCatcher(std::string filename) {
        G4cerr << "Inside setCrossCatcher" << G4endl;
//        G4cerr << m_crCatcher << G4endl;
        m_crCatcher->setFile(filename);
        m_isCrCatcher = true;
    }

    void fill(std::string element, std::string param, double_t x, double_t y, double_t weight) {
//        std::cout << "fill" << std::endl;
            if (m_hists2D->find(HistTuple(element, param, m_resultFolder)) == m_hists2D->end()) {
                if (m_initData->find(param) == m_initData->end()) {
                    std::cerr << "No such param data (" << param << "). Use 'addParams' methord previously.";
                }
                std::vector<HistParams> hp = (*m_initData)[param];
                (*m_hists2D)[HistTuple(element, param, m_resultFolder)] = new My::Hist2D(hp[0].size, hp[0].binMin, hp[0].binMax, \
                        hp[1].size, hp[1].binMin, hp[1].binMax, param, m_resultFolder + "\\" + element);
            }
            (*m_hists2D)[HistTuple(element, param, m_resultFolder)]->fill(x, y, weight);
    }

    void dumpHistograms(int statistic) {
        for (auto it = m_hists1D->begin(); it != m_hists1D->end(); it++) {
            it->second->updateData(statistic);
        }
    }

    const double epsilon = 1E-12;
    void setStartEnergy(double x) {
        (*startEnergy)[0] = x-epsilon;
    }

    void setResFolder(std::string folderName) {
        createResFolder();
        m_resultFolder = folderName;
    }

    void createMessenger() {
        m_messenger = new G4GenericMessenger(this, "/histogram/");
        m_messenger->SetGuidance("Commands for hist manager class");
//        m_messenger->DeclareProperty("resultFolder", m_resultFolder);
        m_messenger->DeclareMethod("resultFolder", G4AnyMethod( &My::HistManager::setResFolder ));

//        void (My::HistManager::*myfunc)(double);
//        /* the ampersand is actually optional */
//        myfunc = &My::HistManager::setStartEnergy;
        m_messenger->DeclareMethodWithUnit("startEnergy", "MeV", G4AnyMethod( &My::HistManager::setStartEnergy ));
    }

    void del() {
        for (auto it = m_hists1D->begin(); it != m_hists1D->end(); it++) {
            delete it->second;
        }
        for (auto it = m_hists2D->begin(); it != m_hists2D->end(); it++) {
            delete it->second;
        }
    }

    void inc(std::string element) {
            if (m_count->find(element) == m_count->end()) {
                (*m_count)[element] = 0;
            }
            m_count->at(element) = m_count->at(element) + 1;
    }

    void setLimits() {

    }

    std::atomic_int32_t numOfFission;

//    std::atomic<double> startEnergy;
//    sf::safe_ptr<double> startEnergy;
    sf::safe_ptr< std::map< int, double > > startEnergy;

    int test_counter = 0;



private:


    HistManager()= default;
    ~HistManager() {
        std::cout << "Inside HistManagerDestr" << std::endl;
        for (auto it = m_hists1D->begin(); it != m_hists1D->end(); it++) {
            delete it->second;
        }
        for (auto it = m_hists2D->begin(); it != m_hists2D->end(); it++) {
            delete it->second;
        }

        if (!m_count->empty()) {
            std::ofstream out(m_resultFolder + "/counts.txt");

            std::vector<std::pair<std::string, int>> pairs;
            for (auto it = m_count->begin(); it != m_count->end(); it++)
                pairs.push_back(*it);

            std::sort(pairs.begin(), pairs.end(), [=](std::pair<std::string, int>& a, std::pair<std::string, int>& b) {
                return a.second > b.second;
            });

            for (auto it = pairs.begin(); it != pairs.end(); it++) {
//                std::cout << "   " << it->first << "              " << it->second << std::endl;
                out << std::setw(8) << it->first << " " << it->second << std::endl;
            }
            out.close();
        }
    }
    HistManager(const HistManager&)= delete;
    HistManager& operator=(const HistManager&)= delete;

    sf::safe_ptr< std::unordered_map< std::string, std::vector<HistParams>   > > m_initData;
//    sf::safe_ptr< std::map< std::pair<std::string, std::string>, My::Hist*   > > m_hists1D;
    sf::safe_ptr< std::unordered_map< HistTuple, My::Hist*, HistTupleHasher > > m_hists1D;
    sf::safe_ptr< std::unordered_map< HistTuple, My::Hist2D*, HistTupleHasher > > m_hists2D;
    sf::safe_ptr< std::map< std::string, unsigned int > > m_count;
    sf::safe_ptr< std::map< std::string, double > > m_LETmap;

    sf::safe_ptr< std::unordered_map< std::string, My::CrossCatcher* > > m_crossCatchers;

    sf::safe_ptr< My::CrossCatcher > m_crCatcher;
    bool m_isCrCatcher = false;

    G4GenericMessenger * m_messenger;
    std::string m_resultFolder = "unnamedResults";
//    std::vector< std::string > m_resultFolders = {{"unnamedResults"}};

    double m_limit; // um
    size_t    m_netSteps;
};

} // namespace My

#endif
