#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLineEdit>

class IndexView : public QGraphicsView
{
	Q_OBJECT
public:
	IndexView(QWidget* parent = 0);

	void setImage(const QImage& im);

	void load(const QList<QPair<QString, QPointF> >& pts); // The points should be from (0, 0) at the bottom left to (1, 1) at the top right.
	QList<QPair<QString, QPointF> > save() const;

protected:
	void wheelEvent(QWheelEvent* e);

	void mousePressEvent(QMouseEvent* e);
	void contextMenuEvent(QContextMenuEvent* e);
	void keyPressEvent(QKeyEvent* e);

private:

	QGraphicsScene* scene;
	QGraphicsPixmapItem* background;

};

