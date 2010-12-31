#include "App.h"

#include <QFile>
#include <QCoreApplication>
#include <QDebug>

App::App(QObject* parent) : QObject(parent)
{
}

void App::TransformOSM(QString inputFile, QString outputFile)
{
	qDebug() << "Opening " << inputFile;
	QFile f(inputFile);
	QFile o(outputFile);
	if (!f.open(QFile::ReadOnly) || !o.open(QFile::WriteOnly | QFile::Truncate))
	{
		return;
	}

	qDebug() << "Parsing XML";

	if (!parser.read(&f, f.size()))
	{
		return;
	}

	qDebug() << "Writing output.";

	qDebug() << "Total nodes: " << parser.nodes().size() << " Total ways: " << parser.ways().size() << " Total atms: " << parser.atms().size() << " Total pubs: " << parser.pubs().size();

	QDataStream out(&o);

	qDebug() << "Writing nodes.";
	out << parser.nodes();
	qDebug() << "Writing ways.";
	out << parser.ways();
	qDebug() << "Writing atms.";
	out << parser.atms();
	qDebug() << "Writing pubs.";
	out << parser.pubs();

	qDebug() << "Done";

	o.close();
}
