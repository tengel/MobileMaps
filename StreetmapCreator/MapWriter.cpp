#include "MapWriter.h"

#include "FileStoreWriter.h"

#include <QBuffer>
#include "PngCrush.h"

QList<QImage> TileImage(const QImage& im, int tileSize)
{
	QList<QImage> r;
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
			r.append(sub);
		}
	}
	return r;
}

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

void WriteInt(QByteArray& ba, int i)
{
	int j = ((i >> 24) & 0xFF) | ((i >> 8) & 0xFF00) | ((i << 8) & 0xFF0000) | ((i << 24) & 0xFF000000);
	ba.append(reinterpret_cast<char*>(&j), 4);
}

bool WriteMap(QString outputFile, const QImage& im,
		  int tileSize, int overviewZoom, int format, bool usePNGCrush, QByteArray index)
{
	if (format != FORMAT_PNG && format != FORMAT_JPEG)
		return false;

	// Possibly fix output filename.
	if (!outputFile.toLower().endsWith(".mapth"))
		outputFile.append(".mapth");

	// They have already said the want to over-write it.
	if (QFile::exists(outputFile))
		QFile::remove(outputFile);

	FileStoreWriter store;

	// Get the shrunk image. TODO: Not sure why I didn't use ShrinkImage. Must not be working.
	QImage imShrunk = im.scaledToWidth(im.width() / overviewZoom, Qt::SmoothTransformation);//ShrinkImage(im, overviewZoom);

	// Tile both images.
	QList<QImage> imTiles = TileImage(im, tileSize);
	QList<QImage> imShrunkTiles = TileImage(imShrunk, tileSize);

	// Create the header file.
	QByteArray dat;

			/** The reading code looks like:

			def.mapWidth = din.readInt(); // in tiles
			def.mapHeight = din.readInt();
			def.tileWidth = din.readInt(); // in pixels
			def.tileHeight = din.readInt();
			def.totalWidth = din.readInt(); // in pixels
			def.totalHeight = din.readInt();
			def.overviewMapWidth = din.readInt(); // in tiles
			def.overviewMapHeight = din.readInt();
			def.overviewTotalWidth = din.readInt(); // in pixels
			def.overviewTotalHeight = din.readInt();
			*/

	int mapWidth = ((im.width()-1) / tileSize)+1; // TODO: Better way of doing this.
	int mapHeight = ((im.height()-1) / tileSize)+1;

	int overviewMapWidth = ((imShrunk.width()-1) / tileSize)+1;
	int overviewMapHeight = ((imShrunk.height()-1) / tileSize)+1;

	WriteInt(dat, mapWidth);
	WriteInt(dat, mapHeight);
	WriteInt(dat, tileSize);
	WriteInt(dat, tileSize);
	WriteInt(dat, im.width());
	WriteInt(dat, im.height());
	WriteInt(dat, overviewMapWidth);
	WriteInt(dat, overviewMapHeight);
	WriteInt(dat, imShrunk.width());
	WriteInt(dat, imShrunk.height());

	store.appendFile(dat);

	// Create the tiles.
	for (int i = 0; i < imTiles.size(); ++i)
	{
		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		if (format == FORMAT_PNG)
		{
			imTiles[i].save(&buffer, "PNG");
			if (usePNGCrush)
				PNGCrush(ba);
		}
		else
		{
			imTiles[i].save(&buffer, "JPEG");
		}
		store.appendFile(ba);
	}
	// Create the overview tiles.
	for (int i = 0; i < imShrunkTiles.size(); ++i)
	{
		QByteArray ba;
		QBuffer buffer(&ba);
		buffer.open(QIODevice::WriteOnly);
		if (format == FORMAT_PNG)
		{
			imShrunkTiles[i].save(&buffer, "PNG");
			if (usePNGCrush)
				PNGCrush(ba);
		}
		else
		{
			imShrunkTiles[i].save(&buffer, "JPEG");
		}
		store.appendFile(ba);
	}

	store.appendFile(index);

	return store.writeToFile(outputFile);
}


