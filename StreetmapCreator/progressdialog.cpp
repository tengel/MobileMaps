#include "progressdialog.h"
#include "ui_progressdialog.h"

#include "StreetmapDownloader.h"
#include "MapWriter.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QMultiHash>

#include <QDebug>

#include "Index.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ProgressDialog)
{
    m_ui->setupUi(this);

	streetmap = new StreetmapDownloader(this);
	connect(streetmap, SIGNAL(finished()), SLOT(streetmapFinished()));
	connect(streetmap, SIGNAL(progressMade()), SLOT(updateProgress()));
	connect(m_ui->buttonBox, SIGNAL(rejected()), streetmap, SLOT(cancel()));
	connect(m_ui->buttonBox, SIGNAL(rejected()), SLOT(close()));

}

ProgressDialog::~ProgressDialog()
{
    delete m_ui;
}

void ProgressDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ProgressDialog::build(QString fileName, LatLong bottomLeft, LatLong topRight, int zoom, bool withIndex)
{
	outputFile = fileName;
	includeStreetIndex = withIndex;
	streetmap->download(LatLongToGridRef(bottomLeft), LatLongToGridRef(topRight), zoom);
}

void ProgressDialog::streetmapFinished()
{
	if (streetmap->error())
	{
		QMessageBox::information(this, "Error downloading streetmap", streetmap->errorMessage());
		return;
	}
	QImage im = streetmap->result();
	if (im.isNull())
	{
		QMessageBox::information(this, "Error downloading streetmap", "Unknown error.");
		return;
	}

	if (!readOSM())
	{
		QMessageBox::information(this, "Error reading OSM info.", "Unknown error.");
		return;
	}

	QByteArray index;
	if (includeStreetIndex)
	{
		if (!createIndex(index, streetmap->resultBottomLeft(), streetmap->resultTopRight()))
		{
			QMessageBox::information(this, "Error creating index.", "Couldn't create index.");
			return;
		}
	}
	else
	{
		index.append('\xFF');
	}

	// Get the filename.
	WriteMap(outputFile, im, 150, 3, FORMAT_PNG, false, index);

	accept();
	deleteLater();
}

void ProgressDialog::updateProgress()
{
	m_ui->progressBarOverall->setValue(streetmap->progress());
}

bool ProgressDialog::readOSM()
{
	QFile fin("output.txt");
	if (!fin.open(QFile::ReadOnly))
		return false;

	QDataStream in(&fin);

	qDebug() << "Reading nodes.";
	in >> nodes;
	qDebug() << "Reading ways.";
	in >> ways;
	qDebug() << "Reading atms.";
	in >> atms;
	qDebug() << "Reading pubs.";
	in >> pubs;
	qDebug() << "Done";

	fin.close();

	return true;
}

bool ProgressDialog::createIndex(QByteArray& index, GridRef bottomLeft, GridRef topRight)
{
	Index idx;

	QMultiHash<QString, GridRef> wayLocations;

	for (int w = 0; w < ways.size(); ++w)
	{
		GridRef pos = ways[w].position(nodes);

		if (pos.easting > bottomLeft.easting && pos.easting < topRight.easting
			&& pos.northing > bottomLeft.northing && pos.northing < topRight.northing)
		{
			wayLocations.insert(ways[w].name(), pos);
		}
	}

	QString currentName = "";
	GridRef currentPos;
	int currentDenom = 0;
	for (QHashIterator<QString, GridRef> i(wayLocations); i.hasNext();)
	{
		i.next();
		if (i.key() == currentName)
		{
			currentPos.easting += i.value().easting;
			currentPos.northing += i.value().northing;
			++currentDenom;
		}
		else
		{
			if (currentDenom > 0)
			{
				GridRef pos(currentPos.easting / currentDenom, currentPos.northing / currentDenom);

				int x = 65535 * (pos.easting - bottomLeft.easting) / (topRight.easting - bottomLeft.easting);
				int y = 65535 * (pos.northing - bottomLeft.northing) / (topRight.northing - bottomLeft.northing);

				if (x < 0) x = 0; if (x > 65535) x = 65535; // Just make sure.
				if (y < 0) y = 0; if (y > 65535) y = 65535;

				idx.add(currentName, x, y);
			}
			currentName = i.key();
			currentPos = i.value();
			currentDenom = 1;
		}
	}
	if (currentDenom > 0)
	{
		GridRef pos(currentPos.easting / currentDenom, currentPos.northing / currentDenom);

		int x = 65535 * (pos.easting - bottomLeft.easting) / (topRight.easting - bottomLeft.easting);
		int y = 65535 * (pos.northing - bottomLeft.northing) / (topRight.northing - bottomLeft.northing);

		if (x < 0) x = 0; if (x > 65535) x = 65535; // Just make sure.
		if (y < 0) y = 0; if (y > 65535) y = 65535;

		idx.add(currentName, x, y);
	}
	index = idx.data();

	return true;
}
