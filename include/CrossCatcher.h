#ifndef CROSSCATCHER_H
#define CROSSCATCHER_H

#include <string>
#include <math.h> // double_t
#include <cstring> // memset size_t
#include <ostream>
#include <stdexcept>

namespace My
{

class CrossCatcher
{
public:

    CrossCatcher();
    ~CrossCatcher();

    CrossCatcher(std::string filename, std::string xUnit = "eV", std::string yUnit = "barn", unsigned int offset = 0);
    friend std::ostream & operator << (std::ostream &out, My::CrossCatcher const & cCatcher);

    double_t getCoef(double_t x);
    void difBins();
    void setFile(std::string filename);

    void scaleX(double_t scale);
    void scaleY(double_t scale);

private:
    void readFunc(std::string func_filename, unsigned int offset = 0);
    void checkOrder();

    double_t *  m_funcX = nullptr;
    double_t *  m_funcY = nullptr;
    size_t      m_funcSize = 0;
    std::string m_xUnit;
    std::string m_yUnit;
    double_t    m_xUnitScale = 1.;
    double_t    m_yUnitScale = 1.;
};


inline std::ostream & operator << (std::ostream &out, My::CrossCatcher const & cCatcher)
{
    out << "x Unit: " << cCatcher.m_xUnit << " * " << cCatcher.m_xUnitScale << std::endl;
    out << "y Unit: " << cCatcher.m_yUnit << " * " << cCatcher.m_yUnitScale << std::endl;
    out << "x array, " << cCatcher.m_funcSize << " elements:" << std::endl;
    out << "[ ";
    for (size_t i = 0; i < cCatcher.m_funcSize; i++) {
        out << cCatcher.m_funcX[i] << " ";
    }
    out << "]" << std::endl;
    out << "y array, " << cCatcher.m_funcSize << " elements:" << std::endl;
    for (size_t i = 0; i < cCatcher.m_funcSize; i++) {
        out << "Y[" << i << "](" <<  cCatcher.m_funcX[i] << ") = " << cCatcher.m_funcY[i] << std::endl;
    }
    return out;
}
} // namespace My

#endif // CROSSCATCHER_H
