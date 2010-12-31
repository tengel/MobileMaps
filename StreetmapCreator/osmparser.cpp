#include "osmparser.h"

OSMParser::OSMParser()
{
}
bool OSMParser::read(QIODevice* dev)
{
	clear();
	nodeList.clear();
	wayList.clear();
	
	setDevice(dev);

	while (!atEnd())
	{
		readNext();

		if (isStartElement())
		{
			if (name() == "osm" && attributes().value("version") == "0.5")
				return readOSM();
			else
				return false; // The file is not an OSM version 0.5 file.
		}
	}
	return true;
}

const QList<Node>& OSMParser::nodes()
{
	return nodeList;
}
const QList<Way>& OSMParser::ways()
{
	return wayList;
}

// Recursive descent parser.
bool OSMParser::readOSM()
{
	Q_ASSERT(isStartElement() && name() == "osm");

	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			return true;

		if (isStartElement())
		{
			if (name() == "relation")
			{
				if (!readRelation())
					return false;
			}
			else if (name() == "node")
			{
				if (!readNode())
					return false;
			}
			else if (name() == "way")
			{
				if (!readWay())
					return false;
			}
			else if (name() == "bound")
			{
				if (!readBound())
					return false;
			}
			else
			{
				Q_ASSERT(false);
				if (!readUnknownElement())
					return false;
			}
		}
	}
	return true;
}

// OSM can contain.
bool OSMParser::readRelation()
{
	Q_ASSERT(isStartElement() && name() == "relation");

	for (int level = 1; level > 0 && !atEnd();) // Skip for now.
	{
		readNext();

		if (isEndElement())
			--level;

		if (isStartElement())
			++level;
	}
	return true;
}
bool OSMParser::readNode()
{
	Q_ASSERT(isStartElement() && name() == "node");

	// Get the id & latitude & longitude attributes.

	if (attributes().hasAttribute("id") && attributes().hasAttribute("lon") && attributes().hasAttribute("lat"))
	{
		LatLong l(attributes().value("lat").toString().toDouble(), attributes().value("lat").toString().toDouble());
		Node n(LatLongToGridRef(l), attributes().value("id").toString().toInt());
		nodeList.append(n);
	}

	// Nodes can contain only tags.
	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (name() == "tag")
			{
				// We don't care about this yet. Tags must be empty.
				readNext();

				Q_ASSERT(isEndElement() && name() == "tag");
			}
			else
			{
				Q_ASSERT(false);
				if (!readUnknownElement())
					return false;
			}
		}
	}
	return true;
}
bool OSMParser::readWay()
{
	Q_ASSERT(isStartElement() && name() == "way");

	Way w;

	// Ways can contain some combinations of tags and nodes.
	while (!atEnd())
	{
		readNext();

		if (isEndElement())
			break;

		if (isStartElement())
		{
			if (name() == "nd")
			{
				if (attributes().hasAttribute("ref"))
				{
					w.addNode(attributes().value("ref").toString().toInt());
				}
				// Node references must be empty.
				readNext();

				Q_ASSERT(isEndElement() && name() == "nd");
			}
			else if (name() == "tag")
			{
				// If the key is name then we shall set it.
				if (attributes().value("k") == "name")
					w.setName(attributes().value("v").toString());

				readNext();

				Q_ASSERT(isEndElement() && name() == "tag");
			}
			else
			{
				if (!readUnknownElement())
					return false;
			}
		}
	}

	if (w.numNodes() > 1)
		wayList.append(w);

	return true;
}
bool OSMParser::readBound()
{
	Q_ASSERT(isStartElement() && name() == "bound");

	readNext(); // Bound must be an empty element.

	Q_ASSERT(isEndElement() && name() == "bound");

	return true;
}

bool OSMParser::readUnknownElement()
{
	Q_ASSERT(isStartElement());

	int level = 1;

	while(!atEnd())
	{
		readNext();

		if (isStartElement())
			++level;
		if (isEndElement())
			--level;

		if (level == 0)
			return true;
	}

	return false;
}
