#include "Way.h"

Way::Way()
{
}

void Way::addNode(int ref)
{
	nodes.append(ref);
}

int Way::numNodes() const
{
	return nodes.size();
}

void Way::setName(const QString& n)
{
	wayName = n;
}
QString Way::name() const
{
	return wayName;
}
GridRef Way::position(const QHash<int, GridRef>& nds)
{
	double easting = 0.0;
	double northing = 0.0;
	int denom = 0;
	for (int i = 0; i < nodes.size(); ++i)
	{
		if (nds.contains(nodes[i]))
		{
			easting += nds[nodes[i]].easting;
			northing += nds[nodes[i]].northing;
			++denom;
		}
	}
	if (denom == 0)
		return GridRef(-INFINITY, -INFINITY);
	return GridRef(easting / denom, northing / denom);
}
