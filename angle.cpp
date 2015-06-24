#include "utils.h"
#include "angle.h"

#include <c++utilities/application/failure.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

using namespace std;
using namespace ApplicationUtilities;

Angle::Angle() :
    m_val(0)
{
}

Angle::Angle(double value, AngularMeasure measure) :
    m_val(0)
{
    switch(measure)
    {
    case AngularMeasure::Radian:
        m_val = value;
        break;
    case AngularMeasure::Degree:
        m_val = value * M_PI / 180.0;
    }
}

Angle::Angle(const string &value, AngularMeasure measure) :
    m_val(0)
{
    switch(measure) {
    case AngularMeasure::Radian:
        m_val += ConversionUtilities::numberFromString<double>(value);
        break;
    case AngularMeasure::Degree:
    {
        string::size_type mpos, spos = string::npos;
        mpos = value.find(':');
        if(mpos == string::npos)
            m_val += ConversionUtilities::numberFromString<double>(value);
        else if(mpos >= (value.length() - 1))
            throw Failure("excepted minutes after ':' in " + value);
        else {
            m_val += ConversionUtilities::numberFromString<double>(value.substr(0, mpos));
            spos = value.find(':', mpos + 1);
            if(spos == string::npos)
                m_val += ConversionUtilities::numberFromString<double>(value.substr(mpos + 1)) / 60.0;
            else if(spos >= (value.length() - 1))
                throw Failure("excepted seconds after second ':'' in " + value);
            else
                m_val += (ConversionUtilities::numberFromString<double>(value.substr(mpos + 1, spos - mpos - 1)) / 60.0)
                        + (ConversionUtilities::numberFromString<double>(value.substr(spos + 1)) / 3600.0);
        }
        m_val = m_val * M_PI / 180.0;
        break;
    }
    }
}

double Angle::degreeValue() const
{
    return m_val * 180.0 / M_PI;
}

double Angle::radianValue() const
{
    return m_val;
}

bool Angle::isNull() const
{
    return m_val == 0.0;
}

void Angle::adjust0To360()
{
    while(m_val < 0) m_val += 2.0 * M_PI;
    while(m_val > 2.0 * M_PI) m_val -= 2.0 * M_PI;
}

void Angle::adjust180To180()
{
    while(m_val > M_PI) m_val -= 2.0 * M_PI;
    while(m_val < -M_PI) m_val += 2.0 * M_PI;
}

void Angle::reverse()
{
    m_val += M_PI;
    adjust0To360();
}

string Angle::toString(OutputForm format) const
{
    stringstream sstream(stringstream::in | stringstream::out);
    sstream << setprecision(9);
    double intpart, fractpart;
    switch(format) {
    case OutputForm::Degrees:
        sstream << degreeValue();
        break;
    case OutputForm::Minutes:
        if(degreeValue() < 0)
            sstream << "-";
        fractpart = modf(fabs(degreeValue()), &intpart);
        fractpart *= 60;
        sstream << intpart << ":" << fractpart;
        break;
    case OutputForm::Seconds:
        if(degreeValue() < 0)
            sstream << "-";
        fractpart = modf(fabs(degreeValue()), &intpart);
        sstream << intpart << ":";
        fractpart *= 60;
        fractpart = modf(fractpart, &intpart);
        fractpart *= 60;
        sstream << intpart << ":" << fractpart;
        break;
    case OutputForm::Radians:
        sstream << radianValue();
    }
    return sstream.str();
}

bool Angle::operator ==(const Angle &other) const
{
    return m_val == other.m_val;
}

bool Angle::operator !=(const Angle &other) const
{
    return m_val != other.m_val;
}

Angle Angle::operator +(const Angle &other) const
{
    return Angle(m_val + other.m_val);
}

Angle Angle::operator -(const Angle &other) const
{
    return Angle(m_val - other.m_val);
}

Angle &Angle::operator +=(const Angle &other)
{
    m_val += other.m_val;
    return *this;
}

Angle &Angle::operator -=(const Angle &other)
{
    m_val -= other.m_val;
    return *this;
}
