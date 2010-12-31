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
