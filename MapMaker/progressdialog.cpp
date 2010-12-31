#include "progressdialog.h"
#include "ui_progressdialog.h"

#include <QCloseEvent>

ProgressDialog::ProgressDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::ProgressDialog)
{
	m_ui->setupUi(this);
}

ProgressDialog::~ProgressDialog()
{
	delete m_ui;
}

void ProgressDialog::changeEvent(QEvent *e)
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
void ProgressDialog::closeEvent(QCloseEvent *event)
{
	event->accept();
}
