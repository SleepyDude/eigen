#include "MyHist2D.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip> // setw()
#include "Utils.h"

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


int My::Hist2D::histcounter = 0;

class FnameException : public std::runtime_error {
public:
    FnameException(std::string filename)
      : std::runtime_error("FnameException")
    {
        std::stringstream ss;
        ss << "Filename " << filename << " does not exist!";
        m_text = ss.str();
    }
    virtual ~FnameException() {}

    virtual const char* what() const throw() {
        return m_text.c_str();
    }
private:
    std::string m_text;
};

class BinOrderException : public std::runtime_error {
public:
    BinOrderException(double_t one, double_t two)
      : std::runtime_error("BinOrderException")
    {
        std::stringstream ss;
        ss << "Wrong bin order!\n";
        ss << one << " >= " << two;
        m_text = ss.str();
    }

    virtual ~BinOrderException() {}
    virtual const char* what() const throw() {
        return m_text.c_str();
    }
private:
    std::string m_text;
};

class TokenException : public std::runtime_error {
public:
    TokenException(std::string bad_token)
      : std::runtime_error("TokenException")
    {
        std::stringstream ss;
        ss << "Bad bin: '" << bad_token << "' should be double type";
        m_text = ss.str();
    }

    virtual ~TokenException() {}
    virtual const char* what() const throw() {
        return m_text.c_str();
    }
private:
    std::string m_text;
};

My::Hist2D::Hist2D(std::string name)
    : m_name(name)
{
    //createMessenger();
    My::Hist2D::histcounter++;
}

My::Hist2D::~Hist2D() {
    // write data in file
    writeInFile(m_name, m_folder);

    delete[] m_binsX;
    delete[] m_binsY;
    for (size_t i = 0; i < m_sizeY; i++) {
        delete[] m_data[i];
    }
    delete[] m_data;
    m_sizeX = 0;
    m_sizeY = 0;
}

My::Hist2D::Hist2D(size_t nbinsX, double_t bin_minX, double_t bin_maxX,
                   size_t nbinsY, double_t bin_minY, double_t bin_maxY,
                   std::string name, std::string folder)
    : m_name(name), m_folder(folder)
{
    m_number = histcounter++;
    m_sizeX = nbinsX;
    m_sizeY = nbinsY;
    m_binsX = new double_t[m_sizeX + 1];
    m_binsY = new double_t[m_sizeY + 1];
    m_data  = new double_t*[m_sizeY];
    for (size_t i = 0; i < m_sizeY; i++) {
        m_data[i] = new double_t[m_sizeX];
    }
    std::vector<double_t> binsX;
    for (size_t i = 0; i < m_sizeX + 1; i++) {
        double_t bin = bin_minX + (bin_maxX - bin_minX) / m_sizeX * i;
        binsX.push_back(bin);
    }
    std::copy(binsX.begin(), binsX.end(), m_binsX);

    std::vector<double_t> binsY;
    for (size_t i = 0; i < m_sizeY + 1; i++) {
        double_t bin = bin_minY + (bin_maxY - bin_minY) / m_sizeY * i;
        binsY.push_back(bin);
    }
    std::copy(binsY.begin(), binsY.end(), m_binsY);

    for (size_t j = 0; j < m_sizeY; j++) {
        for (size_t i = 0; i < m_sizeX; i++) {
            m_data[j][i] = 0;
        }
    }
}

void My::Hist2D::fill(double_t x, double_t y, double_t weight) {
    m_counter++;
    // binary search
    int idx_x, idx_y;
    if (x > m_binsX[0] && x < m_binsX[m_sizeX]) {
        size_t left  = 0;
        size_t right = m_sizeX;
        while(right - left != 1) {
            size_t mid = (left + right) / 2;
            if (m_binsX[mid] < x)
                left = mid;
            else
                right = mid;
        }
        idx_x = left;
    } else {
        idx_x = -1;
    }

    if (y > m_binsY[0] && y < m_binsY[m_sizeY]) {
        size_t left  = 0;
        size_t right = m_sizeY;
        while(right - left != 1) {
            size_t mid = (left + right) / 2;
            if (m_binsY[mid] < y)
                left = mid;
            else
                right = mid;
        }
        idx_y = left;
    } else {
        idx_y = -1;
    }

    if (idx_x >=0 && idx_y >=0) {
        m_data[idx_y][idx_x] += weight;
    }
}

void My::Hist2D::writeInFile(std::string filename, std::string folder) {
//    std::cout << "WRITE IN FILE" << std::endl;
//    std::cout << folder << " " << filename << " " << m_counter << std::endl;
    // make filename if unknown
    if (filename.empty())
        filename = "unknownHist #" + std::to_string(m_number);
    // create folder if necessary
    if (!folder.empty()) {
#if defined(_WIN32)
        int errcode = mkdir(folder.c_str());
#elif !defined(_WIN32)
        int errcode = mkdir(folder.c_str(), 755);
#endif
        if(!errcode || errno == 17) { // 17 - folder already exists, all right
            filename = folder + "\\" + filename;
        } else {
            std::cerr << "Error :  " << strerror(errno) << std::endl;
        }
    }

    std::string newname;
    int files_counter = 0;
//    newname = filename + "0";
    newname = filename;

//    for(;;) {
//        std::ifstream f;
//        f.open(newname + ".txt");
//        if (f.is_open()) {
//          files_counter++;
//          newname = filename + std::to_string(files_counter);
//        } else {
//            break;
//        }
//    }
    filename = newname + ".txt";
    std::ofstream out(filename.c_str());
    out.precision(4);
//    out << "X " << m_sizeX << " bins" << std::endl;
//    if (m_sizeX) {
//        for (size_t i = 0; i < m_sizeX + 1; i++) {
//            out << std::setw(8) << m_binsX[i];
//        }
//    }
//    out << "\n\rY " << m_sizeY << " bins" << std::endl;
//    if (m_sizeY) {
//        for (size_t i = 0; i < m_sizeY + 1; i++) {
//            out << std::setw(8) << m_binsY[i];
//        }
//    }
//    out << "\n\relements" << std::endl;
//    for (size_t j = 0; j < m_sizeY; j++) {
//        for (size_t i = 0; i < m_sizeX; i++) {
//            out << std::setw(8) << m_data[j][i];
//        }
//        out << std::endl;
//    }
    for (size_t j = 0; j < m_sizeY; j++) {
        for (size_t i = 0; i < m_sizeX; i++) {
            out << std::setw(8) << (m_binsY[j] + m_binsY[j+1]) / 2. << " ";
            out << std::setw(8) << (m_binsX[i] + m_binsX[i+1]) / 2. << " ";
            out << std::setw(8) << m_data[j][i] << "\n";
        }
        out << "\r\n";
    }
    out.close();
}

void My::Hist2D::scaleBins(double_t scale) {
    for (size_t i = 0; i < m_sizeX + 1; ++i) {
        m_binsX[i] *= scale;
    }
    for (size_t i = 0; i < m_sizeY + 1; ++i) {
        m_binsY[i] *= scale;
    }
}

void My::Hist2D::scaleData(double_t scale) {
    for (size_t j = 0; j < m_sizeY; ++j) {
        for (size_t i = 0; i < m_sizeX; ++i) {
            m_data[j][i] *= scale;
        }
    }
}

My::Hist2D& My::Hist2D::operator/=(const My::Hist2D& rhs)
{
    for (size_t j = 0; j < m_sizeY; ++j) {
        for (size_t i = 0; i < m_sizeX; ++i) {
            if (rhs.m_data[j][i])
                m_data[j][i] /= rhs.m_data[j][i];
            else
                m_data[j][i] = 0;
        }
    }
  return *this;
}

int My::Hist2D::initialize() {
    if (m_binMaxX <= m_binMinX || m_sizeX <= 0 || m_data || m_binMaxY <= m_binMinY || m_sizeY <= 0) {
        std::cout << "binMaxX " << m_binMaxX << "binMinX " << m_binMinX << "sizeX " << m_sizeX
                  << "binMaxY " << m_binMaxY << "binMinX " << m_binMinY << "sizeY " << m_sizeY;
        return 1;
    }
    m_binsX = new double_t[m_sizeX + 1];
    m_binsY = new double_t[m_sizeY + 1];
    m_data  = new double_t*[m_sizeY];
    for (size_t i = 0; i < m_sizeY; i++) {
        m_data[i] = new double_t[m_sizeX];
    }
    std::vector<double_t> binsX;
    for (size_t i = 0; i < m_sizeX + 1; i++) {
        double_t bin = m_binMinX + (m_binMaxX - m_binMinX) / m_sizeX * i;
        binsX.push_back(bin);
    }
    std::copy(binsX.begin(), binsX.end(), m_binsX);

    std::vector<double_t> binsY;
    for (size_t i = 0; i < m_sizeY + 1; i++) {
        double_t bin = m_binMinY + (m_binMaxY - m_binMinY) / m_sizeY * i;
        binsY.push_back(bin);
    }
    std::copy(binsY.begin(), binsY.end(), m_binsY);
    for (size_t j = 0; j < m_sizeY; j++) {
        for (size_t i = 0; i < m_sizeX; i++) {
            m_data[j][i] = 0;
        }
    }
    return 0;

}

/* void My::Hist2D::createMessenger() {
    m_messenger = new G4GenericMessenger(this, "/hist2d/" + std::to_string(histcounter) + "/");
    m_messenger->SetGuidance("Commands for My::Hist2D class");
    m_messenger->DeclareProperty("setBinsNumX", m_sizeX).SetStates(G4State_PreInit, G4State_Idle);
    m_messenger->DeclareProperty("setBinsNumY", m_sizeY).SetStates(G4State_PreInit, G4State_Idle);
    m_messenger->DeclareProperty("setMinX", m_binMinX).SetStates(G4State_PreInit, G4State_Idle);
    m_messenger->DeclareProperty("setMaxX", m_binMaxX).SetStates(G4State_PreInit, G4State_Idle);
    m_messenger->DeclareProperty("setMinY", m_binMinY).SetStates(G4State_PreInit, G4State_Idle);
    m_messenger->DeclareProperty("setMaxY", m_binMaxY).SetStates(G4State_PreInit, G4State_Idle);
    m_messenger->DeclareProperty("setFolder", m_folder).SetStates(G4State_PreInit, G4State_Idle);
//    m_messenger->DeclareProperty("setName", m_name).SetStates(G4State_PreInit, G4State_Idle);
}*/
