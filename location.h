#ifndef LOCATION_H
#define LOCATION_H

#include "angle.h"

#include <vector>
#include <string>

class Location
{
public:
    enum class GeographicCoordinateSystem
    {
        LatitudeAndLongitude,
        UTMWGS84
    };

    Location();
    Location(const Angle &lat, const Angle &lon);
    explicit Location(const std::string &lat, const std::string &lon, Angle::AngularMeasure measure = Angle::AngularMeasure::Radian);
    explicit Location(const std::string &latitudeAndLongitude, Angle::AngularMeasure measure = Angle::AngularMeasure::Radian);
    ~Location();

    const Angle &latitude() const;
    void setLatitude(const Angle &value);
    const Angle &longitude() const;
    void setLongitude(const Angle &value);
    double elevation() const;
    void setElevation(double value);
    std::string toString(Angle::OutputForm form = Angle::OutputForm::Degrees) const;
    std::string toUtmWgs4String() const;
    bool isEmpty() const;
    double distanceTo(const Location &location) const;
    Angle initialBearingTo(const Location &location) const;
    Angle finalBearingTo(const Location &location) const;
    Location destination(double distance, const Angle &bearing);
    void computeUtmWgs4Coordinates(int &zone, char &zoneDesignator, double &east, double &north) const;
    char computeUtmZoneDesignator() const;
    void setValueByProvidedUtmWgs4Coordinates(const std::string &utmWgs4Coordinates);
    void setValueByProvidedUtmWgs4Coordinates(int zone, char zoneDesignator, double east, double north);
    static Location midpoint(const Location &location1, const Location &location2);
    static double trackLength(const std::vector<Location> &track, bool circle = false);
    static double earthRadius();
    static Angle angularDistance(double distance);
protected:
private:
    Angle m_lat;
    Angle m_lon;
    double m_ele;
    static const double m_er;
};

inline const Angle &Location::latitude() const
{
    return m_lat;
}

inline const Angle &Location::longitude() const
{
    return m_lon;
}

inline double Location::elevation() const
{
    return m_ele;
}

inline void Location::setLatitude(const Angle &value)
{
    m_lat = value;
}

inline void Location::setLongitude(const Angle &value)
{
    m_lon = value;
}

inline void Location::setElevation(double value)
{
    m_ele = value;
}

inline bool Location::isEmpty() const
{
    return m_lat.isNull() && m_lon.isNull() && m_ele == 0.0;
}

#endif // LOCATION_H
