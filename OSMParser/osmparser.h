#ifndef OSMPARSER_H
#define OSMPARSER_H

#include <QXmlStreamReader>
#include <QList>
#include <QHash>

#include "Transform.h"
#include "Way.h"

class OSMParser : QXmlStreamReader
{
public:
	OSMParser();

	bool read(QIODevice* dev, qint64 fileSize = 0);

	const QHash<int, GridRef>& nodes();
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
	bool readBounds(); // Bound is an empty element with attributes origin, box. Both are required.

	bool readUnknownElement();

	QHash<int, GridRef> nodeHash;
	QList<Way> wayList;
	QList<int> pubList;
	QList<int> atmList;

	qint64 xmlFileSize;
};


#endif // OSMPARSER_H
