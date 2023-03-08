#ifndef XSFILEDATA_H
#define XSFILEDATA_H


#include <list>
#include <fstream>
#include <vector>
#include "G4ThreeVector.hh"
#include "Randomize.hh"
#include <functional>

# ifndef M_PI
  # define M_PI 3.14159265358979323846  /* pi */
# endif

struct XSFileData {
  XSFileData() = default;

  XSFileData(std::string /* name */)
  {}

  double GetWeight(double x) {
    int imin = 0;
    int imax = xData.size()-1;
    
    while (imax != imin + 1) {
      int imid = (int)((imin + imax)/2);
      if (x > xData.at(imid))
        imin = imid;
      else
        imax = imid;
    }

    return yData.at(imax);
  }

  double GetWeightInterpolate(double x) {
    int imin = 0;
    int imax = xData.size()-1;

    while (imax != imin + 1) {
      int imid = (int)((imin + imax)/2);
      if (x > xData.at(imid))
        imin = imid;
      else
        imax = imid;
    }

    return yData.at(imin) + ( (yData.at(imax) - yData.at(imin))/(xData.at(imax) - xData.at(imin)) * (x - xData.at(imin)) );
  }

  double  CalculateIntegral(std::function<double(double)> binFunc) {
    double sum = 0;
    // yData[0] для гистограммы всегда == 0 потому что соответствующая ей xData[0] это минимальный бин и ниже него нет веса (он равен 0)
    for (size_t i = 1; i < yData.size(); i++) {
        sum += yData[i]*(binFunc(xData[i]) - binFunc(xData[i-1]));
    }
    return sum;
  }

  double GetBinWidth(double x) {
    int imin = 0;
    int imax = xData.size()-1;

    while (imax != imin + 1) {
      int imid = (int)((imin + imax)/2);
      if (x > xData.at(imid))
        imin = imid;
      else
        imax = imid;
    }

    return xData[imax] - xData[imin];
  }

  int GetBin(double x) {
    int imin = 0;
    int imax = xData.size()-1;

    while (imax != imin + 1) {
      int imid = (int)((imin + imax)/2);
      if (x > xData.at(imid))
        imin = imid;
      else
        imax = imid;
    }

    return imax;
  }

  int readData(G4String filename) {  // На выходе получим массив аргументов х - xData  y - yData и интегральный нормированный массив y - yIntData
    // G4cout << "Start data read" << G4endl;
    std::ifstream inputFile(filename.data());
    if (inputFile.is_open()) {
      double x, y;
      integrate_sum = 0;
      double x_prev = 0.0; // для 0го элемента y = 0 поэтому на самом деле инициализированное значение х_prev не важно
      while (inputFile >> x >> y) {
//        G4cout << "x: " << x << "\n y: " << y << G4endl;
        xData.push_back(x);
        yData.push_back(y);
//        integrate_sum += y*(func(x)-func(x_prev));
        integrate_sum += y;
        x_prev = x;
        yIntData.push_back(integrate_sum);
      }
      inputFile.close();
    } else
      G4cout << "Unable to open file " << filename << G4endl;

    normaliseInteg();

    if (xData.size() != yData.size())
      return -1;

    // G4cout << "End data read" << G4endl;
    return xData.size();
  }

  void normaliseInteg() {
    for (size_t i = 0; i < yData.size(); i++) {
        yIntData[i] = yIntData[i] / integrate_sum;
    }
  }

//  void integrate() {
//      for (size_t i = 1; i < yData.size(); i++) {
//          yData[i] += yData[i-1];
//      }
//      G4cout << "Integrate final value = " << yData[yData.size()-1] << G4endl;
//  }

  G4double sampleValue() {
    G4double ksi = G4UniformRand();
    // binary search
    size_t imin = 0;
    size_t imax = yIntData.size()-1;

    while (imax != imin + 1) {
      int imid = (int)((imin + imax)/2);
      if (ksi > yIntData.at(imid))
        imin = imid;
      else
        imax = imid;
    }

    // Uniform inside x bin
    G4double ksi2 = G4UniformRand();
    G4double x = xData.at(imin) + (xData.at(imax) - xData.at(imin)) * ksi2;

    return x;
  }

  G4double sampleValueCos() {
    G4double ksi = G4UniformRand();
    // binary search
    size_t imin = 0;
    size_t imax = yIntData.size()-1;

    while (imax != imin + 1) {
      int imid = (int)((imin + imax)/2);
      if (ksi > yIntData.at(imid))
        imin = imid;
      else
        imax = imid;
    }

    // Uniform inside x bin
    G4double ksi2 = G4UniformRand();
    G4double sampCos = ( cos(xData.at(imin)/180.0*M_PI) - cos(xData.at(imax)/180.0*M_PI) ) * ksi2 + cos(xData.at(imax)/180.0*M_PI);
    G4double x = acos(sampCos)/M_PI*180;
    return x;
  }


  // G4String m_fname;
  std::vector<double> xData;
  std::vector<double> yData;
  std::vector<double> yIntData;
  G4double integrate_sum = 0;
};

#endif
