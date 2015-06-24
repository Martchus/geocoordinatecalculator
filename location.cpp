#include "location.h"
#include "utils.h"

#include <c++utilities/application/failure.h>

#include <sstream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace ApplicationUtilities;

// WGS84 Parameters
#define WGS84_A		6378137.0               // major axis
#define WGS84_B		6356752.31424518        // minor axis
#define WGS84_F		0.0033528107            // ellipsoid flattening
#define WGS84_E		0.0818191908            // first eccentricity
#define WGS84_EP	0.0820944379        	// second eccentricity

// UTM Parameters
#define UTM_K0		0.9996                  // scale factor
#define UTM_FE		500000.0                // false easting
#define UTM_FN_N	0.0                     // false northing, northern hemisphere
#define UTM_FN_S	10000000.0              // false northing, southern hemisphere
#define UTM_E2		(WGS84_E * WGS84_E) 	// e^2
#define UTM_E4		(UTM_E2 * UTM_E2)		// e^4
#define UTM_E6		(UTM_E4 * UTM_E2)		// e^6
#define UTM_EP2		(UTM_E2 / (1 - UTM_E2))	// e'^2

Location::Location() :
    m_lat(0.0),
    m_lon(0.0),
    m_ele(0.0)
{}

Location::Location(const Angle &latitude, const Angle &lon) :
    m_lat(latitude),
    m_lon(lon),
    m_ele(0.0)
{}

Location::Location(const string &lat, const string &lon, Angle::AngularMeasure measure) :
    m_lat(Angle(lat, measure)),
    m_lon(Angle(lon, measure)),
    m_ele(0.0)
{}

Location::Location(const string &latitudeAndLongitude, Angle::AngularMeasure measure) :
    m_ele(0.0)
{
    string::size_type dpos = latitudeAndLongitude.find(',');
    if(dpos == string::npos)
        throw Failure("Pair of coordinates (latitude and longitude) required.");
    else if(dpos >= (latitudeAndLongitude.length() - 1))
        throw Failure("No second longitude following after comma.");
    else if(latitudeAndLongitude.find(',', dpos + 1) != string::npos)
        throw Failure("More then 2 coordinates given.");
    m_lat = Angle(latitudeAndLongitude.substr(0, dpos), measure);
    m_lon = Angle(latitudeAndLongitude.substr(dpos + 1), measure);
}

Location::~Location()
{}

string Location::toString(Angle::OutputForm form) const
{
    return m_lat.toString(form) + "," + m_lon.toString(form);
}

string Location::toUtmWgs4String() const
{
    int zone;
    char zoneDesignator;
    double east, north;
    computeUtmWgs4Coordinates(zone, zoneDesignator, east, north);
    stringstream ss(stringstream::in | stringstream::out);
    ss << setprecision(0) << fixed;
    ss << zone << zoneDesignator << "E" << east << "N" << north;
    return ss.str();
}

double Location::distanceTo(const Location &location) const
{
    double lat1 = m_lat.radianValue();
    double lon1 = m_lon.radianValue();
    double lat2 = location.m_lat.radianValue();
    double lon2 = location.m_lon.radianValue();
    double latd = lat1 - lat2;
    double lond = lon1 - lon2;
    latd = sin(latd / 2.0);
    lond = sin(lond / 2.0);
    double a = latd * latd + lond * lond * cos(lat1) * cos(lat2);
    return m_er * 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
}

Angle Location::initialBearingTo(const Location &location) const
{
    double lat1 = m_lat.radianValue();
    double lon1 = m_lon.radianValue();
    double lat2 = location.m_lat.radianValue();
    double lon2 = location.m_lon.radianValue();
    double lond = lon2 - lon1;
    double b = atan2(sin(lond) * cos(lat2), cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lond));
    Angle angle(b);
    angle.adjust0To360();
    return angle;
}

Angle Location::finalBearingTo(const Location &location) const
{
    Angle angle(location.initialBearingTo(*this));
    angle.reverse();
    return angle;
}

Location Location::destination(double distance, const Angle &bearing)
{
    double lat1 = m_lat.radianValue();
    double lon1 = m_lon.radianValue();
    double brng = bearing.radianValue();
    double ad = angularDistance(distance).radianValue();
    double lat2 = asin(sin(lat1) * cos(ad) + cos(lat1) * sin(ad) * cos(brng));
    double lon2 = lon1 + atan2(sin(brng) * sin(ad) * cos(lat1), cos(ad) - sin(lat1) * sin(lat2));
    return Location(Angle(lat2), Angle(lon2));
}

void Location::computeUtmWgs4Coordinates(int &zone, char &zoneDesignator, double &east, double &north) const
{
    double a = WGS84_A;
    double eccSquared = UTM_E2;
    double k0 = UTM_K0;

    double latd = m_lat.degreeValue();
    double lond = m_lon.degreeValue();
    double latr = m_lat.radianValue();
    double lonr = m_lon.radianValue();

    zone = int((lond + 180) / 6) + 1;

    if(latd >= 56.0 && latd < 64.0 && lond >= 3.0 && lond < 12.0)
        zone = 32;

    // Special zones for Svalbard
    if(latd >= 72.0 && latd < 84.0)
    {
        if(lond >= 0.0  && lond <  9.0) zone = 31;
        else if(lond >= 9.0  && lond < 21.0) zone = 33;
        else if(lond >= 21.0 && lond < 33.0) zone = 35;
        else if(lond >= 33.0 && lond < 42.0) zone = 37;
     }

    zoneDesignator = computeUtmZoneDesignator();

    // +3 puts origin in middle of zone
    double lonOriginr = Angle((zone - 1) * 6 - 180 + 3, Angle::AngularMeasure::Degree).radianValue();

    double eccPrimeSquared = (eccSquared) / (1 - eccSquared);
    double N = a / sqrt(1 - eccSquared * sin(latr) * sin(latr));
    double T = tan(latr) * tan(latr);
    double C = eccPrimeSquared * cos(latr) * cos(latr);
    double A = cos(latr) * (lonr-lonOriginr);
    double M = a * ((1 - eccSquared / 4 - 3 * eccSquared * eccSquared / 64
                - 5 * eccSquared * eccSquared * eccSquared / 256) * latr
               - (3 * eccSquared / 8 + 3 * eccSquared*eccSquared / 32
                  + 45 * eccSquared * eccSquared * eccSquared / 1024)*sin(2 * latr)
               + (15 * eccSquared * eccSquared / 256
                  + 45 * eccSquared * eccSquared*eccSquared / 1024) * sin(4 * latr)
               - (35 * eccSquared * eccSquared * eccSquared/3072) * sin(6 * latr));

    east = static_cast<double>
          (k0 * N * (A + (1 - T + C) * A * A * A / 6
                 + (5 - 18 * T + T * T + 72 * C - 58 * eccPrimeSquared) * A * A * A * A * A / 120)
           + 500000.0);

    north = static_cast<double>
          (k0 * (M + N * tan(latr)
               * (A * A / 2 + (5 - T + 9 * C + 4 * C * C) * A * A * A *A / 24
                 + (61 - 58 * T + T * T + 600 * C - 330 * eccPrimeSquared) * A * A * A * A * A * A / 720)));

    if(latd < 0)
        north += 10000000.0;
}

Location Location::midpoint(const Location &location1, const Location &location2)
{
    double lat1 = location1.m_lat.radianValue();
    double lon1 = location1.m_lon.radianValue();
    double lat2 = location2.m_lat.radianValue();
    double lon2 = location2.m_lon.radianValue();
    double lond = lon2 - lon1;
    double x = cos(lat2) * cos(lond);
    double y = cos(lat2) * sin(lond);
    return Location(
                Angle(atan2(sin(lat1) + sin(lat2), sqrt((cos(lat1) + x) * (cos(lat1) + x) + y * y))),
                Angle(lon1 + atan2(y, cos(lat1) + x)));
}

double Location::trackLength(const std::vector<Location> &track, bool circle)
{
    if(track.size() < 2)
        throw Failure("At least two locations are required to calculate a distance.");

    const Location *location1 = &track.at(0);
    const Location *location2 = &track.at(1);
    double distance = location1->distanceTo(*location2);

    for(std::vector<Location>::const_iterator i = track.cbegin() + 2, end = track.cend(); i != end; ++i) {
        location1 = location2;
        location2 = &(*i);
        distance += location1->distanceTo(*location2);
    }

    if(circle)
        distance += track.front().distanceTo(track.back());

    return distance;
}

double Location::earthRadius()
{
    return m_er;
}

Angle Location::angularDistance(double distance)
{
    return Angle(distance / m_er);
}

char Location::computeUtmZoneDesignator() const
{
    double l = m_lat.degreeValue();
    if     ((84 >= l) && (l >= 72))  return 'X';
    else if ((72 > l) && (l >= 64))  return 'W';
    else if ((64 > l) && (l >= 56))  return 'V';
    else if ((56 > l) && (l >= 48))  return 'U';
    else if ((48 > l) && (l >= 40))  return 'T';
    else if ((40 > l) && (l >= 32))  return 'S';
    else if ((32 > l) && (l >= 24))  return 'R';
    else if ((24 > l) && (l >= 16))  return 'Q';
    else if ((16 > l) && (l >= 8))   return 'P';
    else if (( 8 > l) && (l >= 0))   return 'N';
    else if (( 0 > l) && (l >= -8))  return 'M';
    else if ((-8 > l) && (l >= -16)) return 'L';
    else if((-16 > l) && (l >= -24)) return 'K';
    else if((-24 > l) && (l >= -32)) return 'J';
    else if((-32 > l) && (l >= -40)) return 'H';
    else if((-40 > l) && (l >= -48)) return 'G';
    else if((-48 > l) && (l >= -56)) return 'F';
    else if((-56 > l) && (l >= -64)) return 'E';
    else if((-64 > l) && (l >= -72)) return 'D';
    else if((-72 > l) && (l >= -80)) return 'C';
    else return '\0';
}

void Location::setValueByProvidedUtmWgs4Coordinates(const string &utmWgs4Coordinates)
{
    string::size_type epos = utmWgs4Coordinates.find('E');
    if(epos != 0 && epos != string::npos) {
        string::size_type npos = utmWgs4Coordinates.find('N', epos);
        if(npos < (utmWgs4Coordinates.length() - 1) && npos != string::npos) {
            int zone = ConversionUtilities::numberFromString<int>(utmWgs4Coordinates.substr(0, epos - 1));
            char zoneDesignator = utmWgs4Coordinates.at(epos - 1);
            double east = ConversionUtilities::numberFromString<double>(utmWgs4Coordinates.substr(epos + 1, npos - epos - 1));
            double north = ConversionUtilities::numberFromString<double>(utmWgs4Coordinates.substr(npos + 1));
            setValueByProvidedUtmWgs4Coordinates(zone, zoneDesignator, east, north);
            return;
        }
    }
    throw Failure("UTM coordinates incomplete.");
}

void Location::setValueByProvidedUtmWgs4Coordinates(int zone, char zoneDesignator, double easting, double northing)
{
    double k0 = UTM_K0;
    double a = WGS84_A;
    double eccSquared = UTM_E2;
    double eccPrimeSquared;
    double e1 = (1 - sqrt(1 - eccSquared)) / (1 + sqrt(1 - eccSquared));

    double x = easting - 500000.0; //remove 500,000 meter offset for longitude
    double y = northing;

    if((zoneDesignator - 'N') < 0)
        //remove 10,000,000 meter offset used for southern hemisphere
        y -= 10000000.0;

    //+3 puts origin in middle of zone
    double longOriginr = Angle((zone - 1) * 6 - 180 + 3, Angle::AngularMeasure::Degree).radianValue();
    eccPrimeSquared = (eccSquared)/(1-eccSquared);

    double M = y / k0;
    double mu = M / (a * (1 - eccSquared / 4 - 3 * eccSquared * eccSquared / 64
                          -5 * eccSquared * eccSquared * eccSquared / 256));

    double phi1Rad = mu + ((3 * e1 / 2 - 27 * e1 * e1 * e1 / 32) * sin(2 * mu)
                           + (21 * e1 * e1 / 16 - 55 * e1 * e1 * e1 * e1 / 32) * sin(4 * mu)
                           + (151 * e1 * e1 * e1 / 96) * sin(6 * mu));

    double N1 = a / sqrt(1 - eccSquared * sin(phi1Rad) * sin(phi1Rad));
    double T1 = tan(phi1Rad) * tan(phi1Rad);
    double C1 = eccPrimeSquared * cos(phi1Rad) * cos(phi1Rad);
    double R1 = a * (1 - eccSquared) / pow(1 - eccSquared * sin(phi1Rad) * sin(phi1Rad), 1.5);
    double D = x / (N1 * k0);

    m_lat = Angle(phi1Rad - ((N1 * tan(phi1Rad) / R1)
                           * (D * D / 2
                              -(5 + 3 * T1 + 10 * C1 - 4 * C1 * C1 - 9 * eccPrimeSquared) * D * D * D * D / 24
                              +(61 + 90 * T1 + 298 * C1 + 45 * T1 * T1 - 252 * eccPrimeSquared
                                -3 * C1 * C1) * D * D * D * D * D * D / 720)));
    m_lat.adjust180To180();
    m_lon = Angle(((D - (1 + 2 * T1 + C1) * D * D * D / 6
                  +(5 - 2 * C1 + 28 * T1 - 3 * C1 * C1 + 8 * eccPrimeSquared + 24 * T1 * T1)
                  * D * D * D * D * D / 120)
                 / cos(phi1Rad)) + longOriginr);
    m_lon.adjust180To180();
}

const double Location::m_er = 6371000.0;
