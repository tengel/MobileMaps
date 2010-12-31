#ifndef STREETMAPDOWNLOADER_H
#define STREETMAPDOWNLOADER_H

#include <QObject>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QList>
#include <QtGui/QImage>

#include "Transform.h"
#include <QtCore/QHash>
#include <QtCore/QSet>

class StreetmapDownloader : public QObject
{
	Q_OBJECT
public:
	StreetmapDownloader(QObject* parent = 0);

	// Implicitly calls cancel. There can only be one download at a time.
	void download(GridRef bottomLeft, GridRef topRight, int zoom);

	// Returns an integer from 0 to 100. Don't rely on this to see when it is finished. Use the finished() signal.
	int progress() const;

	// Returns false if there has been an error.
	bool error() const;
	// Returns the error string of the error.
	QString errorMessage() const;

	// Get the resulting image.
	QImage result() const;
	// Get the position of the result corners.
	GridRef resultBottomLeft() const;
	GridRef resultTopRight() const;

public slots:

	// Cancel current download & clear all results.
	void cancel();

signals:
	// Emitted when finished, whether due to an error or not. Use error() to determine if the
	// process was successful.
	void finished();

	// Something has been done.
	void progressMade();

private slots:
	// The image URL requests.
	void tileURLReply();
	// The image requests.
	void tileReply();

private:
	// This downloads the stuff over HTTP.
	QNetworkAccessManager* manager;

	// This uniquely identifies a tile in the streetmap.co.uk database.
	struct TileID
	{
		// The x and y values are the grid reference (in metres) of the
		// lower left corner of the square. The zoom level is from 0 to 7 with 7 being the most zoomed out.
		TileID(int _x, int _y, int _zoom) : x(_x), y(_y), zoom(_zoom) { }
		TileID() : x(0), y(0), zoom(0) { }

		int x;
		int y;
		int zoom;
		inline bool operator==(const TileID& a) const
		{
			return a.x == x && a.y == y && a.zoom == zoom;
		}
	};
	friend uint qHash(const StreetmapDownloader::TileID& t);


	// A list of tiles for which the GIF URL hasn't been requested.
	QSet<TileID> tilesAwaitingURLRequest;
	// For these tiles the GIF URL has been requested but not recieved.
	QSet<TileID> tilesAwaitingURLResponse;
	// Now the GIF URL has been received, but we haven't actually asked for it yet.
	QSet<TileID> tilesAwaitingImageRequest;
	// For these we have asked for the GIF and are waiting for it.
	QSet<TileID> tilesAwaitingImageResponse;

	// This is the map where we store the GIF URL for each tile.
	QHash<TileID, QString> tileImageURLs;

	// This is the set of completed tiles.
	QHash<TileID, QImage> tileImages;

	// The set of pending network requests. Useful for cancel() and other things.
	QSet<QNetworkReply*> pendingRequests;

	// The area we are trying to download.
	int mapLeft;
	int mapRight;
	int mapBottom;
	int mapTop;
	int mapZoom;

	// The result of the downloading.
	enum
	{
		Idle,
		Downloading,
		DownloadSucceeded,
		DownloadFailed
	} status;
	QString errorString;

	// *** Internal Helper Functions ***

	// Converts 0-7 to the tiles width in metres.
	int tileWidthMetres(int zoom) const;
	// Converts the 0-7 style zoom to 106,110,etc.
	int tileZNumber(int zoom) const;
	// Converts the 106,110, style thing to 0-7 style zoom.
	int tileZoomNumber(int z) const;

	// Request a tile url and move the ID to the appropriate set.
	void requestTileURLs(const QList<TileID>& ids);
	// Request a tile image and move the ID to the next set.
	void requestTileImage(const TileID& id);

	// Deal with the URL response into a list of URLs.
	bool parseURLs(const QByteArray& ba);


};

inline uint qHash(const StreetmapDownloader::TileID& t)
{
	return qHash(t.x + t.y + t.zoom);
}

/*

How to download tiles from the streetmap site.

1. Use the following request to get a list of image URLS:

http://streetmap.co.uk/ajaxtpm.srf?
 tl=528250|181250|528250|181750|528250|1822.... // Coordinate pairs of the tiles.
&z=106 // Zoom level. Levels are: 106 110 115 etc.

This gives a response like:

ItemCount|6
106_529875_180375@9f&type=L&street=Y@TQ28SE90NE.gif@1.336898
106_530125_180375@fd&type=L&street=Y@TQ38SW00NW.gif@1.336898
106_530375_180375@dc&type=L&street=Y@TQ38SW00NE.gif@1.336898
106_530625_180375@dd&type=L&street=Y@TQ38SW10NW.gif@1.336898
106_530875_180375@fc&type=L&street=Y@TQ38SW10NE.gif@1.336898
106_531125_180375@bd&type=L&street=Y@TQ38SW20NW.gif@1.336898
	   1         @      2           @      3       @   4

From which we construct requests like:

http://streetmap.co.uk/img.srf?image=TQ38SW20NW.gif&loc2=bd&type=L&street=Y

That is:

http://streetmap.co.uk/img.srf?image=[3]&loc2=[2]
*/

/*

  Tile sizes are as follows:

  Zoom		Tile size (pixels)
  0			187x187
  1			187x187 or 200x200 (but they both take up the same amount of land) 187 are the london ones.
  2			200x200
  3			200x200
  4			250x250
  5			250x250
  6			200x200
  7			200x200

  Returned zoom numbers are:
  Zoom		Number
  0			106
  1			110
  2			115
  3			120
  4			126
  5			130
  6			140
  7			150

  Physical tile sizes are as follows:

  Zoom		Tile width/height (m)	Offsets
  0			250						125, 375, 625, 875
  1			500						250, 750
  2			500						250, 750
  3			1 000					500
  4			5 000					2 500, 7 500
  5			10 000					5 000
  6			50 000					25 000, 75 000
  7			100 000					50 000

  // Ah! The tiles are zero aligned but you ask for the *centre* of the tile.

  */

#endif // STREETMAPDOWNLOADER_H
