#include "maker.h"

#include <QImage>
#include <QFile>
#include <QBuffer>
#include <vector>
#include <utility>
#include <string>
#include <map>
#include "index.h"
#include "pngcrush.h"
#include "zipfile.h"


using namespace std;

// This makes a vector of tiles of tileSize*tileSize pixels from the big image im. Left to right then top to bottom.
vector<QImage> TileImage(const QImage& im, int tileSize);
// This scales down an image by averaging pixels. It is shrunk by the factor shrinkBy.
QImage ShrinkImage(const QImage& im, int shrinkBy);



// Create a definition file of the type read by the jar.
QByteArray DefinitionFile(int tileHeight, int tileWidth, int totalHeight, int totalWidth, int mapHeight, int mapWidth,
						  int overviewMapHeight, int overviewMapWidth, int overviewTotalHeight, int overviewTotalWidth)
{
	string dat;
	writeInt(dat, tileHeight);
	writeInt(dat, tileWidth);
	writeInt(dat, totalHeight);
	writeInt(dat, totalWidth);
	writeInt(dat, mapHeight);
	writeInt(dat, mapWidth);
	writeInt(dat, overviewMapHeight);
	writeInt(dat, overviewMapWidth);
	writeInt(dat, overviewTotalHeight);
	writeInt(dat, overviewTotalWidth);
	return QByteArray(dat.c_str(), dat.length());
}

QByteArray ManifestFile(QString midletName)
{
	// Build the string.
	QString dat =
			"Manifest-Version: 1.0\n"
			"Ant-Version: Apache Ant 1.7.0\n"
			"Created-By: 1.6.0_0-b11 (Sun Microsystems Inc.)\n"
			"MIDlet-1: " + midletName + ",icon.png,MapApp\n"
			"MIDlet-Vendor: Tim Hutt\n"
			"MIDlet-Name: " + midletName + "\n"
			"MIDlet-Version: 1.0\n"
			"MicroEdition-Configuration: CLDC-1.1\n"
			"MicroEdition-Profile: MIDP-2.0\n";

	// Standard encoding is UTF-8 apparently.
	return dat.toUtf8().constData();
}

QByteArray JADFile(QString jarFileName, int size, QString midletName)
{
	QString dat =
			"MIDlet-1: " + midletName + ",icon.png,MapApp\n"
			"MIDlet-Jar-Size: " + QString::number(size) + "\n"
			"MIDlet-Jar-URL: " + jarFileName + "\n"
			"MIDlet-Name: " + midletName + "\n"
			"MIDlet-Vendor: Tim Hutt\n"
			"MIDlet-Version: 1.0\n"
			"MicroEdition-Configuration: CLDC-1.1\n"
			"MicroEdition-Profile: MIDP-2.0\n";

	// Standard encoding is UTF-8 apparently.
	return dat.toUtf8().constData();
}

// Make the JAR!
// Paramaters are:
//
// inputImage = Filename of the input image to use.
// inputIndex = Filename of the index to use. The file has a list of Name-coordinate pairs. The string can be empty.
// outputFile = The output jar file to create.
// tileSize = pixel size of the tiles.
// overviewZoom = how much to zoom out for the zoomed out view.
// format = PNG or JPEG. Defined somewhere. TODO: Change to enum.
// iconFileName = name of the icon file (PNG file).
// usePNGCrush = whether to crush the tiles or not (if possible).
// midletName = name of the midlet to be shown
// writeJADFile = whether to create a JAD file with the same name as outputFile (but with a .jad extension).

bool Make(QString midletName, QString inputImage, QString inputIndex, QString outputFile,
		  int tileSize, int overviewZoom, int format, QString iconFileName, bool usePNGCrush, bool writeJADFile)
{
	// Possibly fix output filename.
	if (!outputFile.toLower().endsWith(".jar"))
		outputFile.append(".jar");


	// Remove crazy characters from midlet name and limit name length.
	// Also we can't use commas.
	midletName = midletName.toAscii().left(50);
	for (int i = 0; i < midletName.length(); ++i)
		if (!midletName[i].isPrint() || midletName[i] == ',')
			midletName[i] = ' ';

	// Read the image.
	QImage im(inputImage);
	if (im.isNull())
		return false;

	// Read the index.
	Index idx;
	if (!inputIndex.isEmpty())
	{
		// Read stuff.
		QFile file(inputIndex);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return false;

		while (!file.atEnd())
		{
			QByteArray line = file.readLine();
			QString s(line);
			QString x = s.section(' ', 0, 0);
			QString y = s.section(' ', 1, 1);
			QString text = s.section(' ', 2).trimmed();
			bool ok1 = false;
			bool ok2 = false;
			double xd = x.toDouble(&ok1);
			double yd = y.toDouble(&ok2);

			if (!ok1 || !ok2 || text.length() == 0)
				continue;

			xd = xd > 1.0 ? 1.0 : (xd < 0.0 ? 0.0 : xd); // Clamp to between 0 and 1.
			yd = yd > 1.0 ? 1.0 : (yd < 0.0 ? 0.0 : yd);
			unsigned short xs = xd * 65535;
			unsigned short ys = yd * 65535;
			idx.add(text.toAscii().constData(), xs, ys);
		}
	}

	// Get the shrunk image. TODO: Not sure why I didn't use ShrinkImage. Must not be working.
	QImage imShrunk = im.scaledToWidth(im.width() / overviewZoom, Qt::SmoothTransformation);//ShrinkImage(im, overviewZoom);

	// Tile both images.
	vector<QImage> imTiles = TileImage(im, tileSize);
	vector<QImage> imShrunkTiles = TileImage(imShrunk, tileSize);

	// Create all the files to write to the zip.
	vector<QByteArray> files;

	// The the tiles.
	for (unsigned int i = 0; i < imTiles.size(); ++i)
	{
		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		imTiles[i].save(&buffer, "PNG");
		if (usePNGCrush)
			PNGCrush(ba);
		files.push_back(ba);
	}
	for (unsigned int i = 0; i < imShrunkTiles.size(); ++i)
	{
		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		imShrunkTiles[i].save(&buffer, "PNG");
		if (usePNGCrush)
			PNGCrush(ba);
		files.push_back(ba);
	}
	// They have already said the want to over-write it.
	if (QFile::exists(outputFile))
		QFile::remove(outputFile);
	bool success = QFile::copy("MapTemplate.jar", outputFile);
	if (!success)
		return false;

	ZipFile zf;
	if (!zf.Open(outputFile))
		return false;

	// Write all the tiles.
	for (unsigned int i = 0; i < files.size(); ++i)
	{
		if (!zf.AppendFile("tile_" + QString::number(i) + ".png", files[i]))
			return false;
	}

	// The definition file.
	if (!zf.AppendFile("definition.dat", DefinitionFile(tileSize, tileSize, im.height(), im.width(), ((im.height()-1) / tileSize)+1, ((im.width()-1) / tileSize)+1,  ((imShrunk.height()-1) / tileSize)+1, ((imShrunk.width()-1) / tileSize)+1, imShrunk.height(), imShrunk.width())))
		return false;

	// The index.
	string idxData = idx.write();
	if (!zf.AppendFile("index.dat", QByteArray(idxData.c_str(), idxData.length())))
		return false;

	// Write the icon.
	if (!zf.AppendFile("icon.png", iconFileName))
		return false;

	// Write the manifest.
	if (!zf.AppendFile("META-INF/MANIFEST.MF", ManifestFile(midletName)))
		return false;

	if (!zf.Close())
		return false;

	// Write the JAD file.
	if (writeJADFile)
	{
		QString jadFilename = outputFile;
		jadFilename[jadFilename.length()-1] = 'd'; // jar->jad

		QFile jad(jadFilename); // TODO: Error checking.
		jad.open(QFile::WriteOnly | QFile::Truncate);
		int size = QFile(outputFile).size(); // This works even if the file hasn't been opened.
		jad.write(JADFile(outputFile, size, midletName));
		jad.close();
	}
	return true;
}



vector<QImage> TileImage(const QImage& im, int tileSize)
{
	vector<QImage> r;
	int w = im.width();
	int h = im.height();

	for (int x = 0; x < w; x += tileSize)
	{
		for (int y = 0; y < h; y += tileSize)
		{
			int x2 = x + tileSize;
			int y2 = y + tileSize;
			if (x2 > w)
				x2 = w;
			if (y2 > h)
				y2 = h;
			QImage sub = im.copy(x, y, x2 - x, y2 - y);
			r.push_back(sub);
		}
	}
	return r;
}

#include <iostream>

using namespace std;

QImage ShrinkImage(const QImage& im, int shrinkBy)
{
	if (shrinkBy < 1)
		shrinkBy = 1;
	if (shrinkBy > im.width())
		shrinkBy = im.width();
	if (shrinkBy > im.height())
		shrinkBy = im.height();

	QImage r(im.width() / shrinkBy, im.height() / shrinkBy, QImage::Format_RGB32);

	for (int x = 0; x < r.width(); ++x)
	{
		for (int y = 0; y < r.height(); ++y)
		{
			int sumR = 0;
			int sumG = 0;
			int sumB = 0;
			for (int xs = 0; xs < shrinkBy; ++xs)
			{
				for (int ys = 0; ys < shrinkBy; ++ys)
				{
					QRgb p = im.pixel(x * shrinkBy + xs, y * shrinkBy + ys);
					sumR += qRed(p);
					sumG += qGreen(p);
					sumB += qBlue(p);
				}
			}
			r.setPixel(x, y, qRgb(sumR / (shrinkBy * shrinkBy), sumG / (shrinkBy * shrinkBy), sumB / (shrinkBy * shrinkBy)));
		}
	}
	return r;
}


int EstimateSize(QString inputImage, int tileSize, int overviewZoom, int format)
{
	// Read the image size.
	QImage im(inputImage);
	if (im.isNull())
		return -1;

	int width = im.width();
	int height = im.height();

	QFile file(inputImage);
	if (!file.open(QIODevice::ReadOnly))
		return -1;
	int size = file.size();
	file.close();

	bool isJPEG = inputImage.toLower().endsWith("jpg");

	double tileFactor = 1.5 - 0.2 * tileSize / 100;
	if (tileFactor < 1.0)
		tileFactor = 1.0;

	double zoomFactor = 1.0 + 1.0/overviewZoom;

	if (isJPEG)
	{
		if (format == FORMAT_JPEG)
			return size * tileFactor * zoomFactor;

		return 1.5 * tileFactor * size * zoomFactor;
	}

	if (format == FORMAT_JPEG)
		return size * tileFactor * 1.5 * zoomFactor;

	return size * tileFactor * 0.8 * zoomFactor;
}
