#include "dialog.h"
#include "progressdialog.h"
#include "maker.h"
#include "indexeditor.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent, Qt::WFlags flags)
    : QDialog(parent, flags)
{
	ui.setupUi(this);
}

Dialog::~Dialog()
{

}

void Dialog::browseInputImage()
{
	QString current = ui.inputImageEdit->text();
	if (current.isEmpty())
		current = QDir::homePath();

	current = QFileDialog::getOpenFileName(this, "Select Input Image", current, "Images (*.png *.jpg)");
	if (!current.isEmpty())
		ui.inputImageEdit->setText(current);
}

QString removeFilePart(QString s)
{
	int i = s.lastIndexOf('/');
	if (i == -1)
		return s;
	return s.left(i+1);
}

void Dialog::browseInputIndex()
{
	QString current = ui.inputIndexEdit->text();
	if (current.isEmpty())
	{
		QString im = ui.inputImageEdit->text();
		if (im.isEmpty())
			current = QDir::homePath();
		else
			current = removeFilePart(im);
	}

	current = QFileDialog::getOpenFileName(this, "Select Input Index", current, "Indexes (*.txt)");
	if (!current.isEmpty())
		ui.inputIndexEdit->setText(current);
}
void Dialog::browseOutputFile()
{
	QString current = ui.outputFileEdit->text();
	if (current.isEmpty())
	{
		QString im = ui.inputImageEdit->text();
		if (im.isEmpty())
			current = QDir::homePath();
		else
			current = removeFilePart(im);
	}

	current = QFileDialog::getSaveFileName(this, "Select Output File", current, "Midlet (*.jar)");
	if (!current.isEmpty())
		ui.outputFileEdit->setText(current);
}
void Dialog::updateSizeEstimate()
{
//	int est = EstimateSize(ui.inputImageEdit->text(), ui.tileSizeSpin->value(), ui.zoomSpin->value(), ui.PNGRadio->isChecked() ? FORMAT_PNG : FORMAT_JPEG);
//	if (est > 0)
//		ui.estimateSizeLabel->setText(QString::number(est/1024) + " kB");
}
void Dialog::makeMidlet()
{
//	ProgressDialog dlg(this);
//	dlg.exec();
	if (!Make(ui.midletNameEdit->text(), ui.inputImageEdit->text(), ui.inputIndexEdit->text(),
		 ui.outputFileEdit->text(), ui.tileSizeSpin->value(), ui.zoomSpin->value(), FORMAT_PNG,
		 iconFile, ui.usePNGCrushCheck->isChecked(), ui.jadFileCheck->isChecked()))
		QMessageBox::critical(this, "Error", "Sorry, could do it for some reason.");
}
void Dialog::browseIcon()
{
	QString current = iconFile;
	if (current.isEmpty())
		current = QDir::homePath();
	else
		current = removeFilePart(current);

	current = QFileDialog::getOpenFileName(this, "Select Icon", current, "PNG Images (*.png)");
	if (!current.isEmpty())
	{
		iconFile = current;
		ui.iconBtn->setIcon(QIcon(iconFile));
		ui.iconBtn->setText("");
	}
}

void Dialog::editIndex()
{
	IndexEditor dlg(this);
	if (!ui.inputImageEdit->text().isEmpty())
	{
		QImage im(ui.inputImageEdit->text());
		if (!im.isNull())
			dlg.setImage(im);
	}
	dlg.exec();
}
