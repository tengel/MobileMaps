#include "osmparser.h"

#include <QDebug>

OSMParser::OSMParser()
{
}
bool OSMParser::read(QIODevice* dev, qint64 fileSize)
{
	clear();
	nodeHash.clear();
	wayList.clear();
	atmList.clear();
	pubList.clear();
	xmlFileSize = fileSize;
	
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

const QHash<int, GridRef>& OSMParser::nodes()
{
	return nodeHash;
}
const QList<Way>& OSMParser::ways()
{
	return wayList;
}
const QList<int>& OSMParser::pubs()
{
	return pubList;
}
const QList<int>& OSMParser::atms()
{
	return atmList;
}

// Recursive descent parser.
bool OSMParser::readOSM()
{
	Q_ASSERT(isStartElement() && name() == "osm");

	int i = 0;
	while (!atEnd())
	{
		if (++i % 100000 == 0 && xmlFileSize != 0)
			qDebug() << 100 * characterOffset() / xmlFileSize << "%";

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
			else if (name() == "bounds")
			{
				if (!readBounds())
					return false;
			}
			else if (name() == "bound")
			{
				qDebug() << "Warning: Old bound element found.";
				if (!readUnknownElement())
					return false;
			}
			else
			{
				qDebug() << "Unknown element: " << name();
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

	Q_ASSERT(attributes().hasAttribute("id") && attributes().hasAttribute("lon") && attributes().hasAttribute("lat"));

	QString latString = attributes().value("lat").toString();
	QString lonString = attributes().value("lon").toString();
	double lat = attributes().value("lat").toString().toDouble();
	double lon = attributes().value("lon").toString().toDouble();
	int id = attributes().value("id").toString().toInt();
	nodeHash[id] = LatLongToGridRef(LatLong(lat, lon));
	GridRef r = LatLongToGridRef(LatLong(lat, lon));

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
				// Get the key and the value.
				QString key;
				QString value;

				if (attributes().hasAttribute("k"))
					key = attributes().value("k").toString();
				if (attributes().hasAttribute("v"))
					value = attributes().value("v").toString();

				if (key == "amenity" && value == "pub")
					pubList.append(id);
				if (key == "amenity" && (value == "atm" || value == "bank")) // Assume all banks have atms.
					atmList.append(id);

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
bool OSMParser::readBounds()
{
	Q_ASSERT(isStartElement() && name() == "bounds");

	readNext(); // Bounds must be an empty element.

	Q_ASSERT(isEndElement() && name() == "bounds");

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
