#ifndef APP_H
#define APP_H

#include <QObject>
#include "osmparser.h"

class App : public QObject
{
	Q_OBJECT
public:
	App(QObject* parent = 0);

	void TransformOSM(QString inputFile, QString outputFile);

private:
	OSMParser parser;
};

#endif // APP_H
