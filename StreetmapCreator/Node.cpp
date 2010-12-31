#include "Node.h"

Node::Node() : nodeId(-1)
{
}
Node::Node(const GridRef& p, int id) : nodePos(p), nodeId(id)
{
}

int Node::id() const
{
	return nodeId;
}
void Node::setId(int newId)
{
	nodeId = newId;
}

GridRef Node::pos() const
{
	return nodePos;
}
void Node::setPos(const GridRef& newPos)
{
	nodePos = newPos;
}
