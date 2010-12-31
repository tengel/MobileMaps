#ifndef NODE_H
#define NODE_H

#include "Transform.h"

class Node
{
public:
	Node();
	Node(const GridRef& p, int id = -1);

	int id() const;
	void setId(int newId);

	GridRef pos() const;
	void setPos(const GridRef& newPos);

private:
	GridRef nodePos;
	int nodeId;
};

#endif // NODE_H
