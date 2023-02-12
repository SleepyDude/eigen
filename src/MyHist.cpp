#include "MyHist.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip> // setw()
#include "Utils.h"
//#include "CrossCatcher.h"

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
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif


int My::Hist::histcounter = 0;

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


My::Hist::Hist() {
    //createMessenger();
}

My::Hist::~Hist() {
    // write data in file
    writeInFile(m_name, m_folder);

    delete[] m_bins;
    delete[] m_data;
    delete[] m_counter;
    m_size = 0;
    m_overflow = 0;

    delete[] m_funcX;
    delete[] m_funcY;
    m_funcSize = 0;
}

My::Hist::Hist(std::string bins_filename, std::string name, std::string folder)
    : m_name(name), m_folder(folder)
{
    m_number = My::Hist::histcounter++;
    readBins(bins_filename);
    checkOrder();
    m_state = Init;
}




void My::Hist::readBins(std::string bins_filename)
{
    std::vector<double> bins;

    std::ifstream inputFileStream(bins_filename.c_str());
    if (inputFileStream.fail())
        throw FnameException(bins_filename);

    for (std::string line; std::getline(inputFileStream, line);) {
        std::vector<std::string> tokens = ::split(line, ' ');
        for (auto t : tokens) {
            try {
                bins.push_back(std::stod(t));
            } catch (const std::exception& e) {
                throw TokenException(t);
            }
        }
    }
    m_size = bins.size() - 1;
    m_bins = new double_t[m_size + 1];
    m_data = new double_t[m_size];
    std::copy(bins.begin(), bins.end(), m_bins);
    memset(m_data, 0, m_size*sizeof(double_t));
}

My::Hist::Hist(size_t nbins, double_t bin_min, double_t bin_max, std::string name, std::string folder, My::ScaleType scaletype, std::string scaleFile)
    : m_name(name), m_folder(folder)
{
    m_number = My::Hist::histcounter++;

    std::vector<double> bins;
    if (scaletype == linSc) {
        m_size = nbins;
        m_bins = new double_t[m_size + 1];
        m_data = new double_t[m_size];
        std::cout << "in lin scale in Hist" << std::endl;
        for (size_t i = 0; i < m_size + 1; i++) {
            double_t bin = bin_min + (bin_max - bin_min) / m_size * i;
            bins.push_back(bin);
        }
    } else if (scaletype == logSc) {
        m_size = nbins;
        m_bins = new double_t[m_size + 1];
        m_data = new double_t[m_size];
        std::cout << "in log scale in Hist" << std::endl;
        double_t step = (log(bin_max) - log(bin_min)) / m_size;
        double_t logmin = log(bin_min);
        for (size_t i = 0; i < m_size + 1; i++) {
            double_t bin = exp(logmin + step * i);
            bins.push_back(bin);
        }
    } else if (scaletype == customSc) {
//        std::cout << "in custom scale in Hist" << std::endl;
        std::ifstream inputFileStream(scaleFile.c_str());
        if (inputFileStream.fail())
            throw FnameException(scaleFile);

        for (std::string line; std::getline(inputFileStream, line);) {
            std::vector<std::string> tokens = ::split(line, ' ');
            for (auto t : tokens) {
                try {
                    bins.push_back(std::stod(t));
                } catch (const std::exception& e) {
                    throw TokenException(t);
                }
            }
        }
        m_size = bins.size() - 1;
        m_bins = new double_t[m_size + 1];
        m_data = new double_t[m_size];
    }

    std::copy(bins.begin(), bins.end(), m_bins);
    memset(m_data, 0, m_size*sizeof(double_t));

    m_state = Init;
}

void My::Hist::checkOrder() {
    // m_bins has m_size + 1 elements
    for (size_t i = 0; i < m_size; i++) {
        if ( m_bins[i] >= m_bins[i+1] )
            throw BinOrderException(m_bins[i], m_bins[i+1]);
    }
}

void My::Hist::fill(double_t x, double_t weight) {
    // binary search
//    std::cout << "e = " << x << " weight in Hist = " << weight << std::endl;
    double_t coef = 1.;
    if (m_funcSize) {
        // Binary search coefficient in func array
        size_t left  = 0;
        size_t right = m_funcSize - 1;
        while(right - left != 1) {
            size_t mid = (left + right) / 2;
            if (m_funcX[mid] < x)
                left = mid;
            else
                right = mid;
        }

        // linear interpolation
        double_t k = (m_funcY[right] - m_funcY[left]) / (m_funcX[right] - m_funcX[left]);
        coef = k * (x - m_funcX[left]) + m_funcY[left];
    }

    if (x > m_bins[0] && x < m_bins[m_size]) {
        size_t left  = 0;
        size_t right = m_size;
        while(right - left != 1) {
            size_t mid = (left + right) / 2;
            if (m_bins[mid] < x)
                left = mid;
            else
                right = mid;
        }
        m_data[left] += weight * coef;
//        std::cout << "Insert bin " << static_cast<int>(left) << std::endl;
        m_activeBins.insert(static_cast<int>(left)); // 27.11.2021 Добавление функционала для работы с разреженными гистограммами
    } else if (x > m_bins[m_size]){
        m_overflow += weight * coef;
//        std::cout << "Overflow " << m_overflow << std::endl;
    }
}

void My::Hist::fill(My::Hist const & other) {
    if (m_size == other.m_size) {
        for (size_t i = 0; i <= m_size; i++) {
            m_data[i] += other.m_data[i];
        }
        m_overflow += other.m_overflow;
//        std::cout << "overflow summ = " << m_overflow <<std::endl;

    }
}

void My::Hist::clearData() {
//    for (size_t i = 0; i <= m_size; i++) {
//        m_data[i] = 0;
//    }
//    std::cout << "Clear data size: " << m_size << std::endl;
    memset(m_data, 0, m_size*sizeof(double_t)); // 27.11.2021

    m_overflow = 0;
    m_activeBins.clear(); // 27.11.2021 Добавление функционала для работы с разреженными гистограммами
}

void My::Hist::sqrData() {
    for (size_t i = 0; i <= m_size; i++) {
        m_data[i] *= m_data[i];
    }
//    std::cout << "overflow before = " << m_overflow <<std::endl;
    m_overflow *= m_overflow;
//    std::cout << "overflow after  = " << m_overflow <<std::endl;
}

void My::Hist::difBins() {
    for (size_t i = 0; i < m_size; i++) {
        m_data[i] /= (m_bins[i+1] - m_bins[i]);
    }
}

void My::Hist::difBinsLog() {
    for (size_t i = 0; i < m_size; i++) {
        m_data[i] /= log(m_bins[i+1] / m_bins[i]);
    }
}

void My::Hist::setDivStatus(DivideStatus divStatus) {
    m_divStatus = divStatus;
}

void My::Hist::setScaleData(double_t scale) {
    m_lazyDataScale = scale;
}

void My::Hist::difBins2() {
    for (size_t i = 0; i < m_size; i++) {
        m_data[i] /= 3.1415*(m_bins[i+1]*m_bins[i+1] - m_bins[i]*m_bins[i]);
    }
}

void My::Hist::writeInFile() {
    writeInFile(m_name, m_folder);
}

void My::Hist::writeInFile(std::string filename, std::string folder) {
    if (!m_printFileFlag)
        return;
    std::cout << "Inside writeInFile in My::Hist" << std::endl;

    // Postprocess dividing
    if (m_divStatus == linDiv) {
        difBins();
    } else if (m_divStatus == logDiv) {
        difBinsLog();
    }

    scaleData(m_lazyDataScale);

    // make filename if unknown
    if (filename.empty())
        filename = "unknownHist #" + std::to_string(m_number);
    // create folder if necessary
    if (!folder.empty()) {
#if defined(_WIN32)
        int errcode = mkdir(folder.c_str()); //windows
#else
        int errcode = mkdir(folder.c_str(), 755); //os x
#endif
        if(!errcode || errno == 17) { // 17 - folder already exists, all right
            filename = folder + "\\" + filename; //windows
            // filename = folder + "/" + filename;
        } else {
            std::cerr << "Error :  " << strerror(errno) << std::endl;
        }
    }

    std::string newname;
    //int files_counter = 0;
//    newname = filename + "0";
    newname = filename;

    // Для того чтобы не перезаписывать файлы
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



//    GetFileAttributes(filename); // from winbase.h
//    if(0xffffffff == GetFileAttributes(filename)) {
//        std::cout << "File not found" << std::endl;
//    } else {
//        std::cout << "File found" << std::endl;
//    }

    std::ofstream out(filename.c_str());

    out << std::setw(8) << "#statistic " <<
           std::setw(8) << m_statistic << std::endl;

    out.precision(4);
//    out.flags(out.fixed);
    for (size_t i = 0; i < m_size; i++) {
//        out << std::setw(8) << m_bins[i] << " " << std::setw(8)
//            << std::setw(8) << m_bins[i+1] << " " <<
//               std::setw(8) << m_data[i] << " " << "0.00" << std::endl;
        out << std::setw(8) << m_bins[i+1] << " " << std::setprecision(9) <<
               std::setw(8) << m_data[i] << " " << std::endl;
    }
    out << std::setw(8) << "#overflow " << std::setprecision(9) <<
           std::setw(8) << m_overflow << " " << std::endl;
    out.close();
}

void My::Hist::scaleBins(double_t scale) {
    for (size_t i = 0; i < m_size + 1; ++i) {
        m_bins[i] *= scale;
    }
    for (size_t i = 0; i < m_funcSize; ++i) {
        m_funcX[i] *= scale;
    }
}

void My::Hist::scaleData(double_t scale) {
    for (size_t i = 0; i < m_size; ++i) {
        m_data[i] *= scale;
    }
}

void My::Hist::readFunc(std::string func_filename) {
    std::vector<double> x;
    std::vector<double> y;

    std::ifstream inputFileStream(func_filename.c_str());
    if (inputFileStream.fail())
        throw FnameException(func_filename);

    for (std::string line; std::getline(inputFileStream, line);) {
        std::vector<std::string> tokens = split(line, ' ');
        try {
            x.push_back(std::stod(tokens[0]));
            y.push_back(std::stod(tokens[1]));
        } catch (const std::exception& e) {
            throw TokenException(tokens[0]);
        }
    }
    m_funcSize = x.size();
    m_funcX = new double_t[m_funcSize];
    m_funcY = new double_t[m_funcSize];
    std::copy(x.begin(), x.end(), m_funcX);
    std::copy(y.begin(), y.end(), m_funcY);
}

int My::Hist::createHistogram() {
    if (m_binMax <= m_binMin || m_size <= 0 || m_data) {
        return 1;
    }
    m_number = My::Hist::histcounter++;
    m_bins = new double_t[m_size + 1];
    m_data = new double_t[m_size];
    std::vector<double> bins;
    for (size_t i = 0; i < m_size + 1; i++) {
        double_t bin = m_binMin + (m_binMax - m_binMin) / m_size * i;
        bins.push_back(bin);
    }
    std::copy(bins.begin(), bins.end(), m_bins);
    memset(m_data, 0, m_size*sizeof(double_t));
    m_state = Init;
    return 0;
}

/*void My::Hist::createMessenger() {
    m_messenger = new G4GenericMessenger(this, "/hist/");
    m_messenger->SetGuidance("Commands for My::Hist class");
    m_messenger->DeclareProperty("setBinsNum", m_size).SetStates(G4State_PreInit, G4State_Idle);
    m_messenger->DeclareProperty("setMin", m_binMin).SetStates(G4State_PreInit, G4State_Idle);
    m_messenger->DeclareProperty("setMax", m_binMax).SetStates(G4State_PreInit, G4State_Idle);
    m_messenger->DeclareProperty("setFolder", m_folder).SetStates(G4State_PreInit, G4State_Idle);
    m_messenger->DeclareProperty("setName", m_name).SetStates(G4State_PreInit, G4State_Idle);
}*/

My::State My::Hist::getState() { return m_state; }

void My::Hist::setPrintFlag(bool flag) {
    m_printFileFlag = flag;
}

std::vector< std::pair<double, double> > My::Hist::getDataInVec() {
    std::vector< std::pair<double, double> > data;
    for (auto it = m_activeBins.begin(); it != m_activeBins.end(); it++) {
        double x = (m_bins[*it] + m_bins[(*it) + 1]) / 2.0;
        double y = m_data[*it];
        data.push_back({x, y});
    }
    return data; // Надеюсь что тут будет применяться move семантика
}
