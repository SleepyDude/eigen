#ifndef MyHist2D_h
#define MyHist2D_h 1

#include <string>
#include <math.h> // double_t
#include <cstring> // memset size_t
#include <ostream>
#include <stdexcept>
//#include <G4GenericMessenger.hh>


namespace My
{

class Hist2D
{
public:
    static int histcounter;
    Hist2D() = default;
    Hist2D(std::string name);
    ~Hist2D();

//    Hist(std::string bins_filename, std::string name = "", std::string folder = "");
    Hist2D(size_t nbinsX, double_t bin_minX, double_t bin_maxX,
         size_t nbinsY, double_t bin_minY, double_t bin_maxY,
         std::string name = "", std::string folder = "");

    friend std::ostream & operator << (std::ostream &out, My::Hist2D const & hist);

    void fill(double_t x, double_t y, double_t weight);

    void readFunc(std::string func_filename);
    void writeInFile(std::string filename, std::string folder = "");
    void scaleBins(double_t scale);
    void scaleData(double_t scale);

    Hist2D& operator/=(const Hist2D& rhs);
    int initialize();
//    void createMessenger();

private:
//    void readBins(std::string bins_filename);
//    void checkOrder();

    double_t  * m_binsX = nullptr;
    double_t  * m_binsY = nullptr;
    size_t      m_sizeX = 0;
    size_t      m_sizeY = 0;
    double_t ** m_data  = nullptr;
    double_t    m_binMinX = 0;
    double_t    m_binMaxX = 0;
    double_t    m_binMinY = 0;
    double_t    m_binMaxY = 0;

    std::string m_name   = "";
    std::string m_folder = "";
    int         m_number =  0;
    size_t      m_counter = 0;
//    G4GenericMessenger * m_messenger;
};


inline std::ostream & operator << (std::ostream &out, My::Hist2D const & hist)
{
    if (hist.m_name.empty())
        out << "Unknown 2D Histogram #" << hist.m_number << ":" << std::endl;
    else
        out << "Histogram " << hist.m_name << ":" << std::endl;
    out << "Number of X bins: " << hist.m_sizeX + 1 << " elements:" << std::endl;
    out << "[ ";
    for (size_t i = 0; i < hist.m_sizeX + 1; i++) {
        out << hist.m_binsX[i] << " ";
    }
    out << "]" << std::endl;
    out << "Number of Y bins: " << hist.m_sizeX + 1 << " elements:" << std::endl;
    out << "[ ";
    for (size_t i = 0; i < hist.m_sizeY + 1; i++) {
        out << hist.m_binsY[i] << " ";
    }
    out << "]" << std::endl;
    out << "hist array, " << hist.m_sizeX * hist.m_sizeY << " elements:" << std::endl;
    for (size_t j = 0; j < hist.m_sizeY; j++) {
        for (size_t i = 0; i < hist.m_sizeX; i++) {
            out << "hist[" << i << "][" << j << "] ( x from " <<  hist.m_binsX[i] << " to "
                <<  hist.m_binsX[i+1] << ", y from " << hist.m_binsY[j] << " to "
                <<  hist.m_binsY[j+1] << ") = " << hist.m_data[j][i] << std::endl;
        }
    }
    return out;
}
} // namespace My

#endif
