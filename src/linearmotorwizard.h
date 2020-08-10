#ifndef LINEARMOTORWIZARD_H
#define LINEARMOTORWIZARD_H

#include <stdint.h>
#include <QDialog>

#include <ui_linearmotorwizard.h>

class LinearMotorWizardDlg : public QDialog
{
	Q_OBJECT

public:
	LinearMotorWizardDlg(QWidget *parent = 0);

private:
	Ui::LinearMotorWizardDlg ui;

private slots:
	void CalculateSettings();

signals:
	void sendLinearMotorSettings(uint32_t, uint32_t);
};

#endif // LINEARMOTORWIZARD_H
