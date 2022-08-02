#include "linearmotorwizard.h"

LinearMotorWizardDlg::LinearMotorWizardDlg(QWidget *parent)
{
	Q_UNUSED(parent)
	ui.setupUi(this);

	QObject::connect((this->ui.okBtn), SIGNAL(clicked()), this, SLOT(CalculateSettings()));
}

void LinearMotorWizardDlg::CalculateSettings()
{
	float MPP = this->ui.MPPEdit->value();
	uint32_t CPM = this->ui.CPMEdit->value();

	uint32_t NumberOfPoles = 2;
	uint32_t CountsPerTurn = (float)NumberOfPoles * MPP * (float)CPM;

	emit sendLinearMotorSettings(NumberOfPoles, CountsPerTurn);

	this->close();
}