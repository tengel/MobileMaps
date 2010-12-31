#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QtGui/QDialog>

#include "Transform.h"
#include "StreetmapDownloader.h"

#include "Way.h"

namespace Ui
{
    class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(ProgressDialog)
public:
    explicit ProgressDialog(QWidget *parent = 0);
    virtual ~ProgressDialog();

	void build(QString fileName, LatLong bottomLeft, LatLong topRight, int zoom, bool withIndex);

protected:
	virtual void changeEvent(QEvent* e);

private slots:
	void streetmapFinished();
	void updateProgress();

private:
	Ui::ProgressDialog* m_ui;

	StreetmapDownloader* streetmap;

	QHash<int, GridRef> nodes;
	QList<Way> ways;
	QList<int> atms;
	QList<int> pubs;

	QString outputFile;
	bool includeStreetIndex;

	bool readOSM();
	bool createIndex(QByteArray& index, GridRef bottomLeft, GridRef topRight);
};

#endif // PROGRESSDIALOG_H
