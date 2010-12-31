#pragma once

#include <QtGui/QDialog>
#include "ui_dialog.h"

class Dialog : public QDialog
{
	Q_OBJECT

public:
	Dialog(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Dialog();

private:
	Ui::DialogClass ui;

private slots:
	void browseInputImage();
	void browseInputIndex();
	void browseOutputFile();
	void updateSizeEstimate();

	void browseIcon();

	void makeMidlet();

	void editIndex();
private:
	QString iconFile;
};
