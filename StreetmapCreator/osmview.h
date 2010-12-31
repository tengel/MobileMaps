#ifndef OSMVIEW_H
#define OSMVIEW_H

#include <QtGui/QWidget>
#include <QtCore/QHash>
#include <QtGui/QImage>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QSet>

class OSMView : public QWidget
{
	Q_OBJECT
	Q_DISABLE_COPY(OSMView)
public:
	explicit OSMView(QWidget *parent = 0);
	virtual ~OSMView();

	int getZoom() const;

	QRectF getRegionSelected() const;

	double getX() const; // x in range 0-1
	double getY() const; // y in range 0-1

public slots:
	void zoomIn();
	void zoomOut();

	void setZoom(int level); // 0-18. 18 = zoomed in, 0 = zoomed out.

	// Move by a sensible amount (1/2 screen)
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();

	void setXY(double x, double y); // x and y in range 0-1.

protected:
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	void paintEvent(QPaintEvent* event);


private slots:
	void replyFinished(QNetworkReply*);
private:
	struct TileID
	{
		// For OSM the x and y values are the tile indices. These can go from 0 to 2^zoom-1.
		// The zoom level is from 0-18 with 0 being the most zoomed out.
		int x;
		int y;
		int zoom;
		inline bool operator==(const TileID& a) const
		{
			return a.x == x && a.y == y && a.zoom == zoom;
		}
	};
	friend uint qHash(const OSMView::TileID& t);

	// The x and y positions (i pixels).
	int currentX;
	int currentY;
	int currentZoom;

	QHash<TileID, QImage> tileCache; // If it gets too big we'll remove tiles at random.

	QSet<TileID> desiredTiles; // The current tiles that we want to use to draw the map.
	QSet<TileID> requestedTiles; // The tiles that are currently being fetched.

	QPoint firstCorner; // The first corner (in zoom 18 pixels) of the selected region.
	QPoint secondCorner; // The second corner.

	// The HTTP connection manager.
	QNetworkAccessManager* manager;

	QPoint oldMousePos;

	enum
	{
		TileX = QNetworkRequest::User,
		TileY,
		TileZoom
	};
};


inline uint qHash(const OSMView::TileID& t)
{
	return qHash(t.x + t.y + t.zoom);
}


/* For openstreet map the URLS are of the form:

   http://A.tile.openstreetmap.org/B/C/D.png

	   where:
	A = 'a', 'b', or 'c'. A random tile server. All appear to serve the same files.
	B = the zoom level. '0'-'18'
	C = the x tile number. Tiles are 256px wide.
	D = the y tile number. Values are '0' to 2^B-1

*/

#endif // OSMVIEW_H
