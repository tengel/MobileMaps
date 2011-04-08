#include "Way.h"

Way::Way()
{
}

void Way::addNode(LatLong r)
{
	nodes.push_back(r);
}

LatLong Way::averagePosition() const
{
	double easting = 0.0;
	double northing = 0.0;
	int denom = 0;
	for (int i = 0; i < nodes.size(); ++i)
	{
		easting += nodes[i].longitude;
		northing += nodes[i].latitude;
		++denom;
	}
	if (denom != 0)
	{
		easting /= denom;
		northing /= denom;
	}
	return LatLong(northing, easting);
}

// Used to decide if this is actuall two roads with the same name.
// We assume there aren't three roads of the same name in london.
// There might be one or two, but meh.
double Way::radius() const
{
	if (nodes.empty())
		return 0.0;
	double minX, minY, maxX, maxY;
	minX = maxX = nodes[0].longitude;
	minY = maxY = nodes[0].latitude;

	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].longitude < minX)
			minX = nodes[i].longitude;
		else if (nodes[i].longitude > maxX)
			maxX = nodes[i].longitude;
		if (nodes[i].latitude < minY)
			minY = nodes[i].latitude;
		else if (nodes[i].latitude > maxY)
			maxY = nodes[i].latitude;
	}

	double dx = maxX - minX;
	double dy = maxY - minY;

	return sqrt(dx * dx + dy * dy);
}

// Returns true if the bounding box is taller than it is wide.
// If this is the case then we call the roads 'Foo (north)' and 'Foo (south)'.
// Otherwise east and west.
bool Way::isTall() const
{
	if (nodes.empty())
		return false;
	double minX, minY, maxX, maxY;
	minX = maxX = nodes[0].longitude;
	minY = maxY = nodes[0].latitude;

	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].longitude < minX)
			minX = nodes[i].longitude;
		else if (nodes[i].longitude > maxX)
			maxX = nodes[i].longitude;
		if (nodes[i].latitude < minY)
			minY = nodes[i].latitude;
		else if (nodes[i].latitude > maxY)
			maxY = nodes[i].latitude;
	}

	return (maxX - minX) > (maxY - minY);		
}

// Get the average position of the most southerly/westerly nodes, assuming
// that this is actually two roads.
LatLong Way::getSouthOrWestAveragePosition() const
{
	if (nodes.empty())
		return false;
	double minX, minY, maxX, maxY;
	minX = maxX = nodes[0].longitude;
	minY = maxY = nodes[0].latitude;

	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].longitude < minX)
			minX = nodes[i].longitude;
		else if (nodes[i].longitude > maxX)
			maxX = nodes[i].longitude;
		if (nodes[i].latitude < minY)
			minY = nodes[i].latitude;
		else if (nodes[i].latitude > maxY)
			maxY = nodes[i].latitude;
	}

	bool isTall = (maxX - minX) > (maxY - minY);

	int n = 0;
	LatLong r;
	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if ((!isTall && nodes[i].longitude < (maxX + minX)/2)
				|| (isTall && nodes[i].latitude < (maxY + minY)/2))
		{
			++n;
			r.longitude += nodes[i].longitude;
			r.latitude += nodes[i].latitude;
		}
	}
	r.longitude /= n;
	r.latitude /= n;
	return r;
}

// Get the average position of the most northerly/easterly nodes, assuming
// that this is actually two roads.
LatLong Way::getNorthOrEastAveragePosition() const
{
	if (nodes.empty())
		return false;
	double minX, minY, maxX, maxY;
	minX = maxX = nodes[0].longitude;
	minY = maxY = nodes[0].latitude;

	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].longitude < minX)
			minX = nodes[i].longitude;
		else if (nodes[i].longitude > maxX)
			maxX = nodes[i].longitude;
		if (nodes[i].latitude < minY)
			minY = nodes[i].latitude;
		else if (nodes[i].latitude > maxY)
			maxY = nodes[i].latitude;
	}

	bool isTall = (maxX - minX) > (maxY - minY);

	int n = 0;
	LatLong r;
	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if ((!isTall && nodes[i].longitude >= (maxX + minX)/2)
				|| (isTall && nodes[i].latitude >= (maxY + minY)/2))
		{
			++n;
			r.longitude += nodes[i].longitude;
			r.latitude += nodes[i].latitude;
		}
	}
	r.longitude /= n;
	r.latitude /= n;
	return r;
}


int Way::numNodes() const
{
	return nodes.size();
}
