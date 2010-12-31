#include "indexeditor.h"
#include "ui_indexeditor.h"

#include <QGraphicsPixmapItem>
#include <QDir>
#include <QString>
#include <QList>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

IndexEditor::IndexEditor(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::IndexEditor)
{
	m_ui->setupUi(this);
}

IndexEditor::~IndexEditor()
{
	delete m_ui;
}
void IndexEditor::setImage(const QImage& im)
{
	m_ui->imageView->setImage(im);
}

void IndexEditor::changeEvent(QEvent *e)
{
	switch(e->type())
	{
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}
void IndexEditor::newIndex()
{
}
void IndexEditor::openIndex()
{
	QString current = QDir::homePath();
	QList<QPair<QString, QPointF> > points;

	current = QFileDialog::getOpenFileName(this, "Select Index", current, "Index Files (*.txt)");
	if (!current.isEmpty())
	{
		// Read the file and call IndexView->load().
		QFile file(current);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return;

		while (!file.atEnd())
		{
			QByteArray line = file.readLine();
			QString s(line);
			QString x = s.section(' ', 0, 0);
			QString y = s.section(' ', 1, 1);
			QString text = s.section(' ', 2);
			double xd = x.toDouble();
			double yd = y.toDouble();
			points.append(qMakePair(text, QPointF(xd, yd)));
		}
		m_ui->imageView->load(points);
	}
}
void IndexEditor::saveIndex()
{
	QString current = QDir::homePath();

	current = QFileDialog::getSaveFileName(this, "Select Output File", current, "Index File (*.txt)");
	if (!current.isEmpty())
	{
		if (!current.endsWith(".txt"))
			current.append(".txt");
		QFile file(current);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
			return;

		QList<QPair<QString, QPointF> > points = m_ui->imageView->save();
		QTextStream out(&file);
		for (int i = 0; i < points.size(); ++i)
		{
			out << QString::number(points[i].second.x()) << " " << QString::number(points[i].second.y()) << " "<< points[i].first << "\n";
		}
	}
}

void IndexEditor::addPoint()
{
}
void IndexEditor::delPoint()
{
}
