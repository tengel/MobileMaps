#include "indexwidget.h"

#include <QWheelEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsEllipseItem>
#include <QInputDialog>

#include <iostream>

using namespace std;

IndexView::IndexView(QWidget* parent) : QGraphicsView(parent)
{
	scene = new QGraphicsScene(this);
	setScene(scene);
}

void IndexView::wheelEvent(QWheelEvent* e)
{
	scale(1.0-e->delta()/250.0, 1.0-e->delta()/250.0);
}

void IndexView::mousePressEvent(QMouseEvent* e)
{

	switch (e->button())
	{
	case Qt::RightButton:
		{
			QPointF pos = mapToScene(e->pos());
			if (e->modifiers() & Qt::ShiftModifier)
			{
				QGraphicsItem* it = scene->itemAt(pos);
				if (it && dynamic_cast<QGraphicsTextItem*>(it))
					scene->removeItem(it);
			}
			else
			{
				cout << "Right clicked." << endl;
//				QGraphicsEllipseItem* it = addEllipse(QRectF(-2.0, -2.0, 4.0, 4.0));
//				it->setPos(pos);
	//			QLineEdit* ed = new QLineEdit; // Needs at least Qt 4.4
	//			QGraphicsProxyWidget* edProx = addWidget(ed);
	//			edProx->setPos(e->ButtomDownScenePos(Qt::RightButton));
//				QString text = QInputDialog::getText(NULL, "Index name", "Enter name for this index point");
				QGraphicsTextItem* tit = scene->addText("");
				tit->setTextInteractionFlags(Qt::TextEditorInteraction);
				tit->setFocus();
				tit->setDefaultTextColor(Qt::darkRed);
				tit->setZValue(1.0);
				tit->setPos(pos);
			}
			e->accept();
		}
		break;
	default:
		QGraphicsView::mousePressEvent(e);
		break;
	}
}



void IndexView::contextMenuEvent(QContextMenuEvent* e)
{
}

void IndexView::keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Return)
	{
		scene->clearFocus();
		e->accept();
		return;
	}
	QGraphicsView::keyPressEvent(e);
}

void IndexView::setImage(const QImage& im)
{
	QGraphicsPixmapItem* it = scene->addPixmap(QPixmap::fromImage(im));
	scene->setSceneRect(0, 0, im.width(), im.height());
}

void IndexView::load(const QList<QPair<QString, QPointF> >& pts)
{
	for (int i = 0; i < pts.size(); ++i)
	{
		QGraphicsTextItem* tit = scene->addText(pts[i].first);
		tit->setTextInteractionFlags(Qt::TextEditorInteraction);
		tit->setDefaultTextColor(Qt::darkRed);
		tit->setZValue(1.0);
		QPointF p = pts[i].second;
		QPointF q(p.x() * scene->sceneRect().width(), (1.0-p.y()) * scene->sceneRect().height());
		tit->setPos(q);
	}
}
QList<QPair<QString, QPointF> > IndexView::save() const
{
	QList<QPair<QString, QPointF> > ret;
	QList<QGraphicsItem*> its = scene->items();
	for (int i = 0; i < its.size(); ++i)
	{
		QGraphicsTextItem* it = dynamic_cast<QGraphicsTextItem*>(its[i]);
		if (it)
		{
			QPointF p = it->pos();
			QPointF q(p.x() / scene->sceneRect().width(), 1.0-(p.y()/ scene->sceneRect().height()));
			ret.append(qMakePair(it->toPlainText(), q));
		}
	}
	return ret;
}
