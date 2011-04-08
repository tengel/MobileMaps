#pragma once

#include "Transform.h"

#include <vector>

using namespace std;

class Way
{
public:
	Way();
	
	void addNode(LatLong r);
	
	LatLong averagePosition() const;
	
	// Used to decide if this is actuall two roads with the same name.
	// We assume there aren't three roads of the same name in london.
	// There might be one or two, but meh.
	double radius() const;
	
	// Returns true if the bounding box is taller than it is wide.
	// If this is the case then we call the roads 'Foo (north)' and 'Foo (south)'.
	// Otherwise east and west.
	bool isTall() const;
	
	// Get the average position of the most southerly/westerly nodes, assuming
	// that this is actually two roads.
	LatLong getSouthOrWestAveragePosition() const;
	
	// Get the average position of the most northerly/easterly nodes, assuming
	// that this is actually two roads.
	LatLong getNorthOrEastAveragePosition() const;
	
	int numNodes() const;
private:
	vector<LatLong> nodes;
};
