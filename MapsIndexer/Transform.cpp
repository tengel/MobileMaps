#include "Transform.h"

GridRef LatLongToGridRef(LatLong l)
{
	Ellipse airy1830(6377563.396, 6356256.909);
	double OSGB_F0 = 0.9996012717;
	double N0 = -100000.0;
	double E0 = 400000.0;
	double phi0 = DegToRad(49.0);
	double lambda0 = DegToRad(-2.0);
	double a = airy1830.majorAxis();
	double b = airy1830.minorAxis();
	double eSquared = airy1830.eccentricity();
	double phi = DegToRad(l.latitude);
	double lambda = DegToRad(l.longitude);
	double E = 0.0;
	double N = 0.0;
	double n = (a - b) / (a + b);
	double v = a * OSGB_F0 * pow(1.0 - eSquared * sin(phi) * sin(phi), -0.5);
	double rho = a * OSGB_F0 * (1.0 - eSquared)	* pow(1.0 - eSquared * sin(phi) * sin(phi), -1.5);
	double etaSquared = (v / rho) - 1.0;
	double M =
		(b * OSGB_F0)
			* (((1 + n + ((5.0 / 4.0) * n * n) + ((5.0 / 4.0) * n * n * n)) * (phi - phi0))
				- (((3 * n) + (3 * n * n) + ((21.0 / 8.0) * n * n * n))
					* sin(phi - phi0) * cos(phi + phi0))
				+ ((((15.0 / 8.0) * n * n) + ((15.0 / 8.0) * n * n * n))
					* sin(2.0 * (phi - phi0)) * cos(2.0 * (phi + phi0))) - (((35.0 / 24.0) * n * n * n)
				* sin(3.0 * (phi - phi0)) * cos(3.0 * (phi + phi0))));
	double I = M + N0;
	double II = (v / 2.0) * sin(phi) * cos(phi);
	double III =
		(v / 24.0) * sin(phi) * pow(cos(phi), 3.0)
			* (5.0 - tan(phi) * tan(phi) + (9.0 * etaSquared));
	double IIIA =
		(v / 720.0)
			* sin(phi)
			* pow(cos(phi), 5.0)
			* (61.0 - (58.0 * tan(phi) * tan(phi)) + pow(tan(phi), 4.0));
	double IV = v * cos(phi);
	double V = (v / 6.0) * pow(cos(phi), 3.0) * ((v / rho) - tan(phi) * tan(phi));
	double VI =
		(v / 120.0)
			* pow(cos(phi), 5.0)
			* (5.0 - (18.0 * tan(phi) * tan(phi))
				+ (pow(tan(phi), 4.0)) + (14 * etaSquared) - (58 * tan(phi) * tan(phi) * etaSquared));

	N =
		I + (II * pow(lambda - lambda0, 2.0))
			+ (III * pow(lambda - lambda0, 4.0))
			+ (IIIA * pow(lambda - lambda0, 6.0));
	E =
		E0 + (IV * (lambda - lambda0)) + (V * pow(lambda - lambda0, 3.0))
			+ (VI * pow(lambda - lambda0, 5.0));

	return GridRef(E, N);
}

