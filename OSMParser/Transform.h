#pragma once

#include <cmath>

#include <QDataStream>

using namespace std;

// WGS84 Longitude and latitude.
class LatLong
{
public:
	LatLong(double lat = 0.0, double lon = 0.0) : latitude(lat), longitude(lon) { }
	double latitude;
	double longitude;
};

// An OSGB36 grid reference in metres.
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
LatLong GridRefToLatLong(GridRef r); // TODO

inline QDataStream& operator<<(QDataStream& out, const GridRef& ref)
{
	return (out << ref.easting << ref.northing);
}
inline QDataStream& operator>>(QDataStream& in, GridRef& ref)
{
	return (in >> ref.easting >> ref.northing);
}
