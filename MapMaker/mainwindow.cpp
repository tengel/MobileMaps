#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);
	connect(m_ui->sideButtonGroup, SIGNAL(buttonClicked(int)), m_ui->stackedWidget, SLOT(setCurrentIndex(int)));
}

MainWindow::~MainWindow()
{
	delete m_ui;
}

void MainWindow::changeEvent(QEvent *e)
{
	switch (e->type())
	{
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}
