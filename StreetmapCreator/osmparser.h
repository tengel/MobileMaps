#ifndef OSMPARSER_H
#define OSMPARSER_H

#include <QtCore/QXmlStreamReader>
#include <QtCore/QList>
#include <QtCore/QByteArray>

#include "Transform.h"
#include "Node.h"
#include "Way.h"

class OSMParser : QXmlStreamReader
{
public:
    OSMParser();

	bool read(QIODevice* dev);

	const QList<Node>& nodes();
	const QList<Way>& ways();
	const QList<int>& pubs();
	const QList<int>& atms();

private:

	// Recursive descent parser.
	bool readOSM();

	// OSM can contain.
	bool readRelation();
	bool readNode();
	bool readWay();
	bool readBound(); // Bound is an empty element with attributes origin, box. Both are required.

	bool readUnknownElement();

	QList<Node> nodeList;
	QList<Way> wayList;
};

#endif // OSMPARSER_H
