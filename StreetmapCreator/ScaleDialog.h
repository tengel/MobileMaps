#ifndef SCALEDIALOG_H
#define SCALEDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class ScaleDialog;
}

class ScaleDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(ScaleDialog)
public:
    explicit ScaleDialog(QWidget *parent = 0);
    virtual ~ScaleDialog();

	// Get the scale selected. -1 for none or error. 0-7 are valid values. 0 is the most zoomed.
	int scale() const;

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::ScaleDialog *m_ui;
};

#endif // SCALEDIALOG_H
