#ifndef UTILS_H
#define UTILS_H

#include <c++utilities/application/failure.h>

#include <iomanip>
#include <sstream>

namespace ConversionUtilities
{

template <class T>
T numberFromString(const std::string &value) {
    T result;
    std::stringstream ss(value, std::stringstream::in | std::stringstream::out);
    if(ss >> result && ss.eof()) return result;
    else throw ApplicationUtilities::Failure(value + " is no number");
}

}

#endif // UTILS_H
