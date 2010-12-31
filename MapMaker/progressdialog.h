#pragma once

#include <QtGui/QDialog>

namespace Ui {
    class ProgressDialog;
}

class ProgressDialog : public QDialog
{
	Q_OBJECT
	Q_DISABLE_COPY(ProgressDialog)
public:
	explicit ProgressDialog(QWidget *parent = 0);
	virtual ~ProgressDialog();

protected:
	virtual void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *event);

private:
    Ui::ProgressDialog *m_ui;
};

