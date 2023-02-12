#ifndef MyHist_h
#define MyHist_h 1

#include <string>
#include <math.h> // double_t
#include <cstring> // memset size_t
#include <ostream>
#include <stdexcept>
#include <set> // 27.11.2021 Добавление функционала для работы с разреженными гистограммами
#include <vector> // 27.11.2011


//#include <G4GenericMessenger.hh

// 27.11.2021 Добавление функционала для работы с разреженными гистограммами


namespace My
{

enum ScaleType {
    linSc,
    logSc,
    customSc
};

enum DivideStatus {
    noDiv,
    linDiv,
    logDiv
};

enum State {
    PreInit,
    Init
};

class Hist
{
public:
    static int histcounter;
    Hist();
    ~Hist();

    template <typename It>
    Hist(It first, size_t size)
        : m_bins(new double_t[size]),
          m_data(new double_t[size-1])
    {
        m_size = size - 1;
        std::copy(first, first + size, m_bins);
        memset(m_data, 0, m_size*sizeof(double_t));
        checkOrder();
    }

    Hist(std::string bins_filename, std::string name = "", std::string folder = "");
    Hist(size_t nbins, double_t bin_min, double_t bin_max, std::string name = "", std::string folder = "", ScaleType = linSc, std::string scaleFile = "bins.txt");
    friend std::ostream & operator << (std::ostream &out, My::Hist const & hist);

    void fill(double_t x, double_t weight);
    void fill(Hist const & other);
    void difBins();
    void difBinsLog();
    void difBins2();
    void readFunc(std::string func_filename);
    void writeInFile(std::string filename, std::string folder = "");
    void writeInFile();
    void scaleBins(double_t scale);
    void scaleData(double_t scale);
    int  createHistogram();
    void createMessenger();
    State getState();
    void clearData();
    void sqrData();

    void setPrintFlag(bool flag);

    void setDivStatus(DivideStatus divStatus);
    void setScaleData(double_t scale);

    void updateData(int statistic) {
        m_statistic = statistic;
        writeInFile();
    }

    std::vector< std::pair<double, double> > getDataInVec();

private:
    void readBins(std::string bins_filename);
    void checkOrder();

    double_t * m_bins = nullptr;
    double_t * m_data = nullptr;
    double_t * m_counter = nullptr;
    size_t     m_size = 0;
    double_t   m_overflow = 0;
    double_t   m_binMin = 0;
    double_t   m_binMax = 0;
    int        m_statistic = 0;

    double_t * m_funcX = nullptr;
    double_t * m_funcY = nullptr;
    size_t     m_funcSize = 0;

    std::string m_name   = "";
    std::string m_folder = "";
    int         m_number =  0;

    std::set<int> m_activeBins; // 27.11.2021 Добавление функционала для работы с разреженными гистограммами

//    G4GenericMessenger * m_messenger;
    State        m_state = PreInit;

    DivideStatus m_divStatus = noDiv;
    double_t     m_lazyDataScale = 1.0;

    bool m_printFileFlag = true;
};


inline std::ostream & operator << (std::ostream &out, My::Hist const & hist)
{
    if (hist.m_name.empty())
        out << "Unknown Histogram #" << hist.m_number << ":" << std::endl;
    else
        out << "Histogram " << hist.m_name << ":" << std::endl;
    out << "bins array, " << hist.m_size + 1 << " elements:" << std::endl;
    out << "[ ";
    for (size_t i = 0; i < hist.m_size + 1; i++) {
        out << hist.m_bins[i] << " ";
    }
    out << "]" << std::endl;
    out << "hist array, " << hist.m_size << " elements:" << std::endl;
    for (size_t i = 0; i < hist.m_size; i++) {
        out << "hist[" << i << "](" <<  hist.m_bins[i] << " : "
            <<  hist.m_bins[i+1] << ") = " << hist.m_data[i] << std::endl;
    }
    out << "overflow: " << hist.m_overflow << std::endl;
    return out;
}
} // namespace My

#endif
