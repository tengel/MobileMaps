#include <QtCore/QCoreApplication>

#include "osmparser.h"
#include "App.h"

#include <QFile>
#include <QDataStream>
#include <QDebug>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	App theApp;
	theApp.TransformOSM("england.osm", "output.txt");

	// Debug:
/*
	{
		QFile o("test.dat");
		if (!o.open(QFile::WriteOnly | QFile::Truncate))
		{
			return 2;
		}

		QDataStream out(&o);

		QHash<int, GridRef> a;
		a[4] = GridRef(5, 10);
		a[6] = GridRef(7, 12);

		qDebug() << "Writing nodes.";
		out << a;
		qDebug() << "Done";

		o.close();
	}

	{
		QFile o("test.dat");
		if (!o.open(QFile::ReadOnly))
		{
			return 2;
		}

		QDataStream out(&o);

		qDebug() << "Reading nodes.";
		QHash<int, GridRef> a;
		out >> a;

		qDebug() << "Read: " << a[4].easting << " " << a[4].northing;
		qDebug() << "Read: " << a[6].easting << " " << a[6].northing;
		qDebug() << "Done";

		o.close();
	}





*/





	//return a.exec();
	return 0;
}
