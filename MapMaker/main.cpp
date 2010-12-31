#include <QtGui/QApplication>
#include "dialog.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Dialog w;
	w.show();
//	MainWindow w;
//	w.show();
	return a.exec();
}
