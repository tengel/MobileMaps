#include "osmview.h"
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

/*
  TODO:
  * Load tiles in a spiral from the centre.
  * While tiles are being loaded, see if we have bigger or smaller tiles cached that we can use temporarily
  * Pre-cache tiles from the next zoom level in (and maybe the next out
  * Allow selecting a region. Drag with left mouse button then use gimp-like interface to alter.
  */

const int tileSize = 256;
const int maxZoom = 18;

OSMView::OSMView(QWidget *parent) :
	QWidget(parent)
{
	manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(replyFinished(QNetworkReply*)));

	currentZoom = 2; // Start a bit zoomed in
	currentX = (1 << currentZoom) * tileSize/2;
	currentY = (1 << currentZoom) * tileSize/2;
}

OSMView::~OSMView()
{
}

int OSMView::getZoom() const
{
	return currentZoom;
}

QRectF OSMView::getRegionSelected() const
{
	QRectF r(QPointF(firstCorner) / double(((1 << maxZoom) - 1) * tileSize),
			 QPointF(secondCorner) / double(((1 << maxZoom) - 1) * tileSize));
	return r.normalized();
}

double OSMView::getX() const
{
	return currentX / double(((1 << currentZoom) - 1) * tileSize);
}

double OSMView::getY() const
{
	return currentY / double(((1 << currentZoom) - 1) * tileSize);
}

void OSMView::zoomIn()
{
	if (currentZoom < maxZoom)
	{
		currentX *= 2;
		currentY *= 2;
		++currentZoom;
		update();
	}
}

void OSMView::zoomOut()
{
	if (currentZoom > 0)
	{
		currentX /= 2;
		currentY /= 2;
		--currentZoom;
		update();
	}
}

void OSMView::setZoom(int newZoom)
{
	if (newZoom < 0)
		newZoom = 0;
	if (newZoom > maxZoom)
		newZoom = maxZoom;

	if (currentZoom != newZoom)
	{
		if (newZoom > currentZoom)
		{
			currentX *= 1 << (newZoom - currentZoom);
			currentY *= 1 << (newZoom - currentZoom);
		}
		else
		{
			currentX /= 1 << (newZoom - currentZoom);
			currentY /= 1 << (newZoom - currentZoom);
		}
		currentZoom = newZoom;
		update();
	}
}

void OSMView::moveLeft()
{
	currentX -= width()/2;
	update();
}

void OSMView::moveRight()
{
	currentX += width()/2;
	update();
}

void OSMView::moveUp()
{
	currentY -= height()/2;
	update();
}
void OSMView::moveDown()
{
	currentY += height()/2;
	update();
}

void OSMView::setXY(double x, double y)
{
	currentX = x * ((1 << currentZoom) - 1) * tileSize;
	currentY = y * ((1 << currentZoom) - 1) * tileSize;
	update();
}

void OSMView::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		QPoint diff = event->pos() - oldMousePos;
		oldMousePos = event->pos();
		currentX -= diff.x();
		currentY -= diff.y();
		scroll(diff.x(), diff.y()); // TODO: Check sign
//		update();
	}
	else if (event->buttons() & Qt::RightButton)
	{

		secondCorner = (event->pos() + (QPoint(currentX - width()/2, currentY - height()/2)))
					  * (1 << (maxZoom - currentZoom));

		update();
	}
	event->accept();
}

void OSMView::mousePressEvent(QMouseEvent* event)
{
	oldMousePos = event->pos();

		// Region selection logic.
		// First see if we are clicking in an existing region.

		// If we are then set the move mode approriately.

		// If not set the first corner to our current location.

	if (event->button() == Qt::RightButton)
	{
		firstCorner = (event->pos() + (QPoint(currentX - width()/2, currentY - height()/2)))
					  * (1 << (maxZoom - currentZoom));
		secondCorner = firstCorner;
		update();
	}

	event->accept();
}

void OSMView::mouseReleaseEvent(QMouseEvent* event)
{
	event->accept();
}

void OSMView::wheelEvent(QWheelEvent* event)
{
	// Keep the location under the cursor fixed.
	int offsX = event->pos().x() - width()/2;
	int offsY = event->pos().y() - height()/2;

	if (event->delta() > 0)
	{
		currentX += offsX / 2;
		currentY += offsY / 2;
		zoomIn();
	}
	else if (event->delta() < 0)
	{
		currentX -= offsX;
		currentY -= offsY;
		zoomOut();
	}

	event->accept();
}

void OSMView::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	desiredTiles.clear();

	int left = currentX - width()/2;
	int right = currentX + width()/2;
	int top = currentY - height()/2; // Uses upsidedown coordinate system.
	int bottom = currentY + height()/2;
	for (int x = left/256; x <= right/256; ++x)
	{
		for (int y = top/256; y <= bottom/256; ++y)
		{
			if (x < 0 || y < 0 || x >= (1 << currentZoom) || y >= (1 << currentZoom))
				continue;

			TileID id;
			id.x = x;
			id.y = y;
			id.zoom = currentZoom;

			desiredTiles.insert(id);
			if (tileCache.contains(id))
			{
				painter.drawImage(x*256-left, y*256-top, tileCache.value(id));
			}
			else if (!requestedTiles.contains(id))
			{
				requestedTiles.insert(id);

				QNetworkRequest request;
				QString servers[3];
				servers[0] = "a";
				servers[1] = "b";
				servers[2] = "c";
				QString url = "http://" + servers[rand() % 3] + ".tile.openstreetmap.org/" + QString::number(currentZoom) + "/"
							  + QString::number(x) + "/" + QString::number(y) + ".png";
				request.setUrl(QUrl(url));
				request.setRawHeader("User-Agent", "OSM Qt Widget 0.1 by Tim Hutt");
				request.setAttribute(static_cast<QNetworkRequest::Attribute>(TileX), x);
				request.setAttribute(static_cast<QNetworkRequest::Attribute>(TileY), y);
				request.setAttribute(static_cast<QNetworkRequest::Attribute>(TileZoom), currentZoom);
				manager->get(request);
			}
		}
	}

	// Now draw the selected region.

	QPoint widgetCorner(currentX - width()/2, currentY - height()/2);

	if (firstCorner != secondCorner)
	{
		QRect r(firstCorner / (1 << (maxZoom - currentZoom)) - widgetCorner,
				secondCorner / (1 << (maxZoom - currentZoom)) - widgetCorner);
		r = r.normalized();

		painter.fillRect(r, QColor(255, 255, 200, 100));
		painter.setPen(QColor(0, 0, 0, 100));
		painter.drawRect(r);
	}
}

void OSMView::replyFinished(QNetworkReply* reply)
{
	if (!reply)
		return;

	if (reply->error() == QNetworkReply::NoError)
	{
		QImage im;
		QByteArray data = reply->readAll();
		if (im.loadFromData(data))
		{
			TileID id;
			id.x = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(TileX)).toInt();
			id.y = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(TileY)).toInt();
			id.zoom = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(TileZoom)).toInt();

			requestedTiles.remove(id);

			tileCache.insert(id, im);

			update();
		}
	}
	delete reply;
}
