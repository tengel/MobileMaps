#pragma once

#include <cmath>

using namespace std;

class LatLong
{
public:
	LatLong(double lat = 0.0, double lon = 0.0) : latitude(lat), longitude(lon) { }
	double latitude;
	double longitude;
};

class GridRef
{
public:
	GridRef(double e = 0.0, double n = 0.0) : easting(e), northing(n) { }
	double easting;
	double northing;
};

class Ellipse
{
public:
	Ellipse(double maj, double min)
	{
		major = maj;
		minor = min;
		ecc = ((maj * maj) - (min * min)) / (maj * maj);
	}
	
	double majorAxis() const { return major; }
	double minorAxis() const { return minor; }
	double eccentricity() const { return ecc; }
	
private:
	double major;
	double minor;
	double ecc;
};

inline double DegToRad(double d)
{
	return d * 0.0174532925;
}

GridRef LatLongToGridRef(LatLong l);

