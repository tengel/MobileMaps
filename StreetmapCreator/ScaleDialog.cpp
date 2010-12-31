#include "ScaleDialog.h"
#include "ui_ScaleDialog.h"

ScaleDialog::ScaleDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ScaleDialog)
{
    m_ui->setupUi(this);
}

ScaleDialog::~ScaleDialog()
{
    delete m_ui;
}

void ScaleDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

int ScaleDialog::scale() const
{
	// TODO: Put the buttons in a button group. This will suffice for now.
	if (m_ui->scale0Btn->isChecked())
		return 0;
	if (m_ui->scale1Btn->isChecked())
		return 1;
	if (m_ui->scale2Btn->isChecked())
		return 2;
	if (m_ui->scale3Btn->isChecked())
		return 3;
	if (m_ui->scale4Btn->isChecked())
		return 4;
	if (m_ui->scale5Btn->isChecked())
		return 5;
	if (m_ui->scale6Btn->isChecked())
		return 6;
	if (m_ui->scale7Btn->isChecked())
		return 7;
	return -1;
}
