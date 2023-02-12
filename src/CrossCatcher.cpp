#include "CrossCatcher.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip> // setw()
#include "Utils.h"

class FnameException : public std::runtime_error {
public:
    FnameException(std::string filename)
      : std::runtime_error("FnameException")
    {
        std::stringstream ss;
        ss << "Filename " << filename << " does not exist!";
        m_text = ss.str();
    }

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

    virtual const char* what() const throw() {
        return m_text.c_str();
    }
private:
    std::string m_text;
};

My::CrossCatcher::CrossCatcher() {
    std::cout << "In cross catcher constructor" << std::endl;
}

void My::CrossCatcher::setFile(std::string filename) {
    std::cout << "Inside setFile in CrossCatcher, filename: " << filename << std::endl;
    readFunc(filename, 0);
}

My::CrossCatcher::~CrossCatcher() {
    delete[] m_funcX;
    delete[] m_funcY;
    m_funcSize = 0;
}

My::CrossCatcher::CrossCatcher(std::string filename, std::string xUnit, std::string yUnit, unsigned int offset)
    : m_xUnit(xUnit), m_yUnit(yUnit)
{
    readFunc(filename, offset);
}

void My::CrossCatcher::readFunc(std::string func_filename, unsigned int offset) {
    std::vector<double> x;
    std::vector<double> y;
    std::cerr << "in readFunc" << std::endl;
    std::ifstream inputFileStream(func_filename.c_str());
//    std::cerr << "in readFunc" << std::endl;
    if (inputFileStream.fail()) {
        std::cerr << "Filestream fail in CrossCatcher" << std::endl;
        throw FnameException(func_filename);
    }
    for (std::string line; std::getline(inputFileStream, line);) {
        if (offset) {
            offset--;
            continue;
        }
//        std::cerr << line << std::endl;
        std::vector<std::string> tokens = split(line, ' ');
//        std::cerr << "'" << tokens[0] << "' '" << tokens[1] << "'" << std::endl;
//        double d1 = std::stod(tokens[0]);
//        std::cerr << d1 << std::endl;
//        double d2 = std::stod(tokens[1]);
//        std::cerr << d2 << std::endl;
        try {
            x.push_back(std::stod(tokens[0]));
            y.push_back(std::stod(tokens[1]));
//            std::cerr << tokens[0] << " " << tokens[1] << std::endl;
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

void My::CrossCatcher::checkOrder() {
    // m_bins has m_size + 1 elements
    for (size_t i = 0; i < m_funcSize - 1; i++) {
        if ( m_funcX[i] >= m_funcX[i+1] )
            throw BinOrderException(m_funcX[i], m_funcX[i+1]);
    }
}

double_t My::CrossCatcher::getCoef(double_t x) {
    double_t coef = 1.;
    if (m_funcSize) {
        if (x > m_funcX[0] && x < m_funcX[m_funcSize - 1]) {
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
        } else if (x < m_funcX[0]) {
            return m_funcY[0];
        } else if (x > m_funcX[m_funcSize - 1]) {
            return m_funcY[m_funcSize - 1];
        }
    }
    return coef;
}

void My::CrossCatcher::scaleX(double_t scale) {
    for (size_t i = 0; i < m_funcSize; ++i) {
        m_funcX[i] *= scale;
    }
    m_xUnitScale *= 1./scale;
}

void My::CrossCatcher::scaleY(double_t scale) {
    for (size_t i = 0; i < m_funcSize; ++i) {
        m_funcY[i] *= scale;
    }
    m_yUnitScale *= 1./scale;
}
