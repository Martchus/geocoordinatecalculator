#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "./angle.h"
#include "./location.h"

#include <iostream>
#include <string>
#include <vector>

enum class SystemForLocations { LatitudeLongitude, UTMWGS84 };

extern Angle::AngularMeasure inputAngularMeasure;
extern Angle::OutputForm outputFormForAngles;
extern SystemForLocations inputSystemForLocations;
extern SystemForLocations outputSystemForLocations;

int main(int argc, char *argv[]);

Location locationFromString(const std::string &userInput);
std::vector<Location> locationsFromFile(const std::string &path);
void printAngleFormatInfo(std::ostream &os);
void printConversion(const std::string &coordinates);
void printDistance(const std::string &locationstr1, const std::string &locationstr2);
void printDistance(double distance);
void printTrackLength(const std::string &filePath, bool circle = false);
void printBearing(const std::string &locationstr1, const std::string &locationstr2);
void printFinalBearing(const std::string &locationstr1, const std::string &locationstr2);
void printMidpoint(const std::string &locationstr1, const std::string &locationstr2);
void printDestination(const std::string &locationstr, const std::string &distancestr, const std::string &bearingstr);
void printLocation(const Location &location);
void printMapsLink(const std::string &filePath);

#endif // MAIN_H_INCLUDED
