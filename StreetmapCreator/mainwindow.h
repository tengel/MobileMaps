#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	Ui::MainWindowClass *ui;

private slots:
	void on_actionExit_triggered();
	void on_actionMakeMap_triggered();
	void on_actionAbout_triggered();
};

#endif // MAINWINDOW_H
