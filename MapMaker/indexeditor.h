#ifndef INDEXEDITOR_H
#define INDEXEDITOR_H

#include <QtGui/QDialog>
#include <QGraphicsScene>

namespace Ui {
    class IndexEditor;
}

class IndexEditor : public QDialog
{
	Q_OBJECT
	Q_DISABLE_COPY(IndexEditor)
public:
	explicit IndexEditor(QWidget *parent = 0);
	virtual ~IndexEditor();

	void setImage(const QImage& im);

protected:
	virtual void changeEvent(QEvent *e);

private:
	Ui::IndexEditor *m_ui;
	QGraphicsScene* scene;
private slots:
	void newIndex();
	void openIndex();
	void saveIndex();

	void addPoint();
	void delPoint();
};

#endif // INDEXEDITOR_H
