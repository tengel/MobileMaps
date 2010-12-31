#include "Way.h"

Way::Way()
{
}

void Way::addNode(GridRef r)
{
	nodes.push_back(r);
}

GridRef Way::averagePosition() const
{
	double easting = 0.0;
	double northing = 0.0;
	int denom = 0;
	for (int i = 0; i < nodes.size(); ++i)
	{
		easting += nodes[i].easting;
		northing += nodes[i].northing;
		++denom;
	}
	if (denom != 0)
	{
		easting /= denom;
		northing /= denom;
	}
	return GridRef(easting, northing);
}

// Used to decide if this is actuall two roads with the same name.
// We assume there aren't three roads of the same name in london.
// There might be one or two, but meh.
double Way::radius() const
{
	if (nodes.empty())
		return 0.0;
	double minX, minY, maxX, maxY;
	minX = maxX = nodes[0].easting;
	minY = maxY = nodes[0].northing;

	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].easting < minX)
			minX = nodes[i].easting;
		else if (nodes[i].easting > maxX)
			maxX = nodes[i].easting;
		if (nodes[i].northing < minY)
			minY = nodes[i].northing;
		else if (nodes[i].northing > maxY)
			maxY = nodes[i].northing;
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
	minX = maxX = nodes[0].easting;
	minY = maxY = nodes[0].northing;

	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].easting < minX)
			minX = nodes[i].easting;
		else if (nodes[i].easting > maxX)
			maxX = nodes[i].easting;
		if (nodes[i].northing < minY)
			minY = nodes[i].northing;
		else if (nodes[i].northing > maxY)
			maxY = nodes[i].northing;
	}

	return (maxX - minX) > (maxY - minY);		
}

// Get the average position of the most southerly/westerly nodes, assuming
// that this is actually two roads.
GridRef Way::getSouthOrWestAveragePosition() const
{
	if (nodes.empty())
		return false;
	double minX, minY, maxX, maxY;
	minX = maxX = nodes[0].easting;
	minY = maxY = nodes[0].northing;

	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].easting < minX)
			minX = nodes[i].easting;
		else if (nodes[i].easting > maxX)
			maxX = nodes[i].easting;
		if (nodes[i].northing < minY)
			minY = nodes[i].northing;
		else if (nodes[i].northing > maxY)
			maxY = nodes[i].northing;
	}

	bool isTall = (maxX - minX) > (maxY - minY);

	int n = 0;
	GridRef r;
	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if ((!isTall && nodes[i].easting < (maxX + minX)/2)
				|| (isTall && nodes[i].northing < (maxY + minY)/2))
		{
			++n;
			r.easting += nodes[i].easting;
			r.northing += nodes[i].northing;
		}
	}
	r.easting /= n;
	r.northing /= n;
	return r;
}

// Get the average position of the most northerly/easterly nodes, assuming
// that this is actually two roads.
GridRef Way::getNorthOrEastAveragePosition() const
{
	if (nodes.empty())
		return false;
	double minX, minY, maxX, maxY;
	minX = maxX = nodes[0].easting;
	minY = maxY = nodes[0].northing;

	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].easting < minX)
			minX = nodes[i].easting;
		else if (nodes[i].easting > maxX)
			maxX = nodes[i].easting;
		if (nodes[i].northing < minY)
			minY = nodes[i].northing;
		else if (nodes[i].northing > maxY)
			maxY = nodes[i].northing;
	}

	bool isTall = (maxX - minX) > (maxY - minY);

	int n = 0;
	GridRef r;
	for (unsigned int i = 0; i < nodes.size(); ++i)
	{
		if ((!isTall && nodes[i].easting >= (maxX + minX)/2)
				|| (isTall && nodes[i].northing >= (maxY + minY)/2))
		{
			++n;
			r.easting += nodes[i].easting;
			r.northing += nodes[i].northing;
		}
	}
	r.easting /= n;
	r.northing /= n;
	return r;
}


int Way::numNodes() const
{
	return nodes.size();
}
