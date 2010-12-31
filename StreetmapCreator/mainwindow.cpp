#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>

#include "ScaleDialog.h"
#include "progressdialog.h"

#include <cmath>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
	QApplication::aboutQt();
}

const double pi = M_PI;

void MainWindow::on_actionMakeMap_triggered()
{
	// Woo start the stuff!
	QRectF region = ui->osmWidget->getRegionSelected();

	double l = (region.left() - 0.5) * 360.0;
	double r = (region.right() - 0.5) * 360.0;

	// This isn't correct because of the mercator projection used.
	double t = (0.5 - region.top()) * 360.0;
	double b = (0.5 - region.bottom()) * 360.0;

	// Invert mercator projection.
	t = atan(sinh(t * pi / 180.0)) * 180.0 / pi;
	b = atan(sinh(b * pi / 180.0)) * 180.0 / pi;

	ScaleDialog scaleDlg(this);
	if (scaleDlg.exec() == QDialog::Rejected)
		return;

	int zoom = scaleDlg.scale();
	if (zoom < 0 || zoom > 7)
	{
		QMessageBox::information(this, "Internal Error", "Invalid scale selected.");
		return;
	}

	bool withIndex = QMessageBox::question(this, "Street Index", "Include street index? Not recommended for maps bigger than London.", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;

	QString outputFileName = QFileDialog::getSaveFileName(this, "Save Map", "/home/t", "MapTH Files (*.mapth)");
	if (outputFileName.isNull())
		return;

	ProgressDialog* proDlg = new ProgressDialog(this);

	proDlg->build(outputFileName, LatLong(b, l), LatLong(t, r), zoom, withIndex);
	proDlg->show();
}

void MainWindow::on_actionExit_triggered()
{
	close();
}
