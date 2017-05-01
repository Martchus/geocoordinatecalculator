#ifndef COORDINATE_H
#define COORDINATE_H

#include <string>

class Angle {
public:
    enum class AngularMeasure { Radian, Degree };

    enum class OutputForm { Degrees, Minutes, Seconds, Radians };

    Angle();
    Angle(double value, AngularMeasure measure = AngularMeasure::Radian);
    explicit Angle(const std::string &value, AngularMeasure measure = AngularMeasure::Radian);
    double degreeValue() const;
    double radianValue() const;
    bool isNull() const;
    void adjust0To360();
    void adjust180To180();
    void reverse();
    std::string toString() const;
    std::string toString(OutputForm format) const;

    bool operator==(const Angle &other) const;
    bool operator!=(const Angle &other) const;
    Angle operator+(const Angle &other) const;
    Angle operator-(const Angle &other) const;
    Angle &operator+=(const Angle &other);
    Angle &operator-=(const Angle &other);

private:
    double m_val;
};

#endif // COORDINATE_H
