#include "StreetmapDownloader.h"

#include <QtGui/QMessageBox>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QPainter>
#include <QDebug>

const int maxUrlsPerRequest = 50;

StreetmapDownloader::StreetmapDownloader(QObject* parent) : QObject(parent)
{
	manager = new QNetworkAccessManager(this);
	mapZoom = 0;
	mapLeft = 0;
	mapRight = 0;
	mapTop = 0;
	mapBottom = 0;
	status = Idle;
}
void StreetmapDownloader::download(GridRef bottomLeft, GridRef topRight, int zoom)
{
	// Cancel pending requests. This should return the class to its virgin state.
	cancel();

	mapZoom = zoom;
	if (mapZoom < 0)
		mapZoom = 0;
	if (mapZoom > 7)
		mapZoom = 7;


	int tileSize = tileWidthMetres(mapZoom);

	// Expand to next square.

	mapLeft = int(bottomLeft.easting) / tileSize;
	mapBottom = int(bottomLeft.northing) / tileSize;
	mapRight = (int(topRight.easting) / tileSize) + 1;
	mapTop = (int(topRight.northing) / tileSize) + 1;
	if (mapRight <= mapLeft)
		mapRight = mapLeft + 1;
	if (mapTop <= mapBottom)
		mapTop = mapBottom + 1;

	mapLeft *= tileSize;
	mapRight *= tileSize;
	mapTop *= tileSize;
	mapBottom *= tileSize;

	for (int x = mapLeft; x < mapRight; x += tileSize)
	{
		for (int y = mapBottom; y < mapTop; y += tileSize)
		{
			tilesAwaitingURLRequest.insert(TileID(x, y, mapZoom));
		}
	}

	// Get the ball rolling by requesting the first set of URLS. We'll limit it to a 100 per GET.

	QList<TileID> initialRequest;
	QSetIterator<TileID> it(tilesAwaitingURLRequest);
	for (int r = 0; it.hasNext() && r < maxUrlsPerRequest; ++r)
	{
		initialRequest.append(it.next());
	}

	requestTileURLs(initialRequest);

	status = Downloading;
}
int StreetmapDownloader::progress() const
{
	int s1 = tilesAwaitingURLRequest.size();
	int s2 = tilesAwaitingURLResponse.size();
	int s3 = tilesAwaitingImageRequest.size();
	int s4 = tilesAwaitingImageResponse.size();

	int s5 = tileImages.size();

	return 100 * (s2*1 + s3*2 + s4*2 + s5*10) / ((s1 + s2  + s3 + s4 + s5) * 10 + 1); // Avoid devide by zero.
}

bool StreetmapDownloader::error() const
{
	return status == DownloadFailed;
}
QString StreetmapDownloader::errorMessage() const
{
	return errorString;
}
QImage StreetmapDownloader::result() const
{
	// First determine the tile size (in pixels).

	int tileSize = 0;
	switch (mapZoom)
	{
	case 0:
		tileSize = 187;
		break;
	case 1:
		// Special case. We must see if there are any tiles with the size 187. If there are
		// use that size, otherwise use 200.
		tileSize = 200;
		for (QHashIterator<TileID, QImage> it(tileImages); it.hasNext();)
		{
			if (it.next().value().width() == 187)
			{
				tileSize = 187;
				break;
			}
		}
		break;
	case 2:
		tileSize = 200;
		break;
	case 3:
		tileSize = 200;
		break;
	case 4:
		tileSize = 250;
		break;
	case 5:
		tileSize = 250;
		break;
	case 6:
		tileSize = 200;
		break;
	case 7:
		tileSize = 200;
		break;
	}

	Q_ASSERT(tileSize != 0);

	int tileSizeMetres = tileWidthMetres(mapZoom);


	int xTiles = ((mapRight - mapLeft) / tileSizeMetres);
	int yTiles = ((mapTop - mapBottom) / tileSizeMetres);

	QImage im(tileSize * xTiles, tileSize * yTiles, QImage::Format_RGB32);

	QPainter p(&im);
	// hasNext() is more like hasCurrent(), and next() is more like currentAndAdvance().
	for (QHashIterator<TileID, QImage> it(tileImages); it.hasNext();)
	{
		it.next();
		int x = (it.key().x - mapLeft) / tileSizeMetres;
		int y = (it.key().y - mapBottom) / tileSizeMetres;

		if (it.value().width() == tileSize)
			p.drawImage(x * tileSize, (yTiles - y - 1) * tileSize, it.value());
		else // Might need to resize for zoom=1
			p.drawImage(x * tileSize, (yTiles - y - 1) * tileSize, it.value().scaledToWidth(tileSize, Qt::SmoothTransformation));
	}
	p.end(); // Not sure this is necessary but it can't hurt. It *might* be necessary if drawing is done in another thread or something crazy.
	return im;
}
void StreetmapDownloader::cancel()
{
	tilesAwaitingURLRequest.clear();
	tilesAwaitingURLResponse.clear();
	tilesAwaitingImageRequest.clear();
	tilesAwaitingImageResponse.clear();
	tileImageURLs.clear();
	tileImages.clear();
	for (QSetIterator<QNetworkReply*> it(pendingRequests); it.hasNext();)
	{
		QNetworkReply* r = it.next();
		delete r;
	}
	pendingRequests.clear();

	mapZoom = 0;
	mapLeft = 0;
	mapRight = 0;
	mapTop = 0;
	mapBottom = 0;

	status = Idle;
	errorString.clear();;
}

bool StreetmapDownloader::parseURLs(const QByteArray& ba)
{
	QString strData(ba);
	QStringList lines = strData.split('\n');
	// There's now a blank line for some reason.
	lines.removeFirst();
	if (lines.size() < 1)
		return true; // Or should that be false?
	// Check the first line says ItemCount|<number>.
	if (!lines[0].startsWith("ItemCount|"))
	{
		status = DownloadFailed;
		errorString = "Got URL response that didn't start ItemCount|\n" + strData;
		emit finished();
		return false;
	}

	for (int i = 1; i < lines.size(); ++i)
	{
		QString line = lines[i].trimmed();
		if (line.isEmpty())
			continue;
		QStringList sections = line.split('@');
		if (sections.size() != 4)
		{
			status = DownloadFailed;
			errorString = "Got URL response line that didn't have four sections of @s: line "
						  + QString::number(i) + " Data: " + strData;
			emit finished();
			return false;
		}

		TileID id;
		id.zoom = tileZoomNumber(sections[0].section('_', 0, 0).toInt());
		id.x = sections[0].section('_', 1, 1).toInt() - tileWidthMetres(mapZoom)/2;
		id.y = sections[0].section('_', 2, 2).toInt() - tileWidthMetres(mapZoom)/2;

		if (id.x == 0 || id.zoom == 0)
		{
			status = DownloadFailed;
			errorString = "Got URL weird response: "
						  + QString::number(i) + " Data: " + strData;
			emit finished();
			return false;

		}

		if (!tilesAwaitingURLResponse.contains(id))
		{
			status = DownloadFailed;
			errorString = "Seem to have got a tile ID that we didn't ask for.";
//			emit finished();
	//		return false;
		}

		tilesAwaitingURLResponse.remove(id);
		qDebug() << "Inserting: " << id.x << " " << id.y << " "<< id.zoom;
		tilesAwaitingImageRequest.insert(id);

		tileImageURLs[id] = "http://streetmap.co.uk/img.srf?image=" + sections[2] + "&loc2=" + sections[1];
	}
	return true;
}


void StreetmapDownloader::tileURLReply()
{
	QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
	if (!reply)
		return;
	pendingRequests.remove(reply);
	QByteArray data = reply->readAll();
	delete reply;

	// Parse the reply.
	if (!parseURLs(data))
		return;

	// Next request.
	if (!tilesAwaitingURLRequest.empty())
	{
		// Request more tiles URLs.
		QList<TileID> urlRequest;
		QSetIterator<TileID> it(tilesAwaitingURLRequest);
		for (int r = 0; it.hasNext() && r < maxUrlsPerRequest; ++r)
		{
			urlRequest.append(it.next());
		}

		requestTileURLs(urlRequest);
	}
	else
	{
		// Request the first tile image.
		Q_ASSERT(tilesAwaitingURLResponse.empty());
		Q_ASSERT(!tilesAwaitingImageRequest.empty());
		TileID id = *(tilesAwaitingImageRequest.begin());
		qDebug() << "Requesting: " << id.x << " " << id.y << " "<< id.zoom;
		requestTileImage(id);
	}

	emit progressMade();
}
void StreetmapDownloader::tileReply()
{
	QNetworkReply* reply = dynamic_cast<QNetworkReply*>(sender());
	if (!reply)
		return;
	pendingRequests.remove(reply);
	QByteArray data = reply->readAll();

	TileID id;
	id.x = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 0)).toInt();
	id.y = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1)).toInt();
	id.zoom = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 2)).toInt();


	QImage im = QImage::fromData(data, "GIF");
	if (im.isNull())
	{
		errorString = "There was an error downloading an image. The data size is " + QString::number(data.size()) + " and the received data was: " + QString(data)
					  + " The reply says: " + reply->errorString();
		status = DownloadFailed;
		emit finished();
		return;
	}
	delete reply;

	Q_ASSERT(tilesAwaitingImageResponse.contains(id));

	tilesAwaitingImageResponse.remove(id);

	tileImages[id] = im;

	if (!tilesAwaitingImageRequest.empty())
	{
		TileID id = *(tilesAwaitingImageRequest.begin());
		qDebug() << "Requesting: " << id.x << " " << id.y << " "<< id.zoom;
		requestTileImage(id);
	}
	else
	{
		status = DownloadSucceeded;
		// We're done!
		emit finished();
		return;
	}

	emit progressMade();
}

// Converts 0-7 to the tiles width in metres.
int StreetmapDownloader::tileWidthMetres(int zoom) const
{
	switch (zoom)
	{
	case 0:
		return 250;
	case 1:
		return 500;
	case 2:
		return 500;
	case 3:
		return 1000;
	case 4:
		return 5000;
	case 5:
		return 10000;
	case 6:
		return 50000;
	case 7:
		return 100000;
	}
	return 0;
}
// Converts the 0-7 style zoom to 106,110,etc.
int StreetmapDownloader::tileZNumber(int zoom) const
{
	switch (zoom)
	{
	case 0:
		return 106;
	case 1:
		return 110;
	case 2:
		return 115;
	case 3:
		return 120;
	case 4:
		return 126;
	case 5:
		return 130;
	case 6:
		return 140;
	case 7:
		return 150;
	}
	return -1;
}
// Converts the 106,110, style thing to 0-7 style zoom.
int StreetmapDownloader::tileZoomNumber(int z) const
{
	switch (z)
	{
	case 106:
		return 0;
	case 110:
		return 1;
	case 115:
		return 2;
	case 120:
		return 3;
	case 126:
		return 4;
	case 130:
		return 5;
	case 140:
		return 6;
	case 150:
		return 7;
	}
	return -1;
}

// Request a tile url and move the ID to the appropriate set.
void StreetmapDownloader::requestTileURLs(const QList<TileID>& ids)
{
	QString url = "http://streetmap.co.uk/ajaxtpm.srf?tl=";

	for (int i = 0; i < ids.size(); ++i)
	{
		Q_ASSERT(tilesAwaitingURLRequest.contains(ids[i]));
		tilesAwaitingURLRequest.remove(ids[i]);
		tilesAwaitingURLResponse.insert(ids[i]);
		if (i != 0)
			url += "|";
		url += QString::number(ids[i].x + tileWidthMetres(mapZoom)/2)
			   + "|" + QString::number(ids[i].y + tileWidthMetres(mapZoom)/2);
	}
	url += "&z=" + QString::number(tileZNumber(mapZoom));

	QNetworkRequest request;
	request.setUrl(QUrl(url));

	request.setRawHeader("User-Agent", "MSIE 9");
	QNetworkReply* reply = manager->get(request);
	connect(reply, SIGNAL(finished()), SLOT(tileURLReply()));
	pendingRequests.insert(reply);
}
// Request a tile image and move the ID to the next set.
void StreetmapDownloader::requestTileImage(const TileID& id)
{
	Q_ASSERT(tilesAwaitingImageRequest.contains(id));
	Q_ASSERT(tileImageURLs.contains(id));

	tilesAwaitingImageRequest.remove(id);
	tilesAwaitingImageResponse.insert(id);

	if (!tileImageURLs.contains(id))
	{
		status = DownloadFailed;
		errorString = "Trying to get id without url: " + QString::number(id.x) + ", " + QString::number(id.y) + ", " + QString::number(id.zoom);
		emit finished();
		return;
	}

	QNetworkRequest request;
	request.setUrl(QUrl(tileImageURLs[id]));
	request.setRawHeader("User-Agent", "MSIE 9");
	request.setAttribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 0), id.x); // So we know which tile each response is for.
	request.setAttribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1), id.y);
	request.setAttribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 2), id.zoom);
	QNetworkReply* reply = manager->get(request);
	connect(reply, SIGNAL(finished()), SLOT(tileReply()));
	pendingRequests.insert(reply);
}
GridRef StreetmapDownloader::resultBottomLeft() const
{
	return GridRef(mapLeft, mapBottom);
}
GridRef StreetmapDownloader::resultTopRight() const
{
	return GridRef(mapRight, mapTop);
}
