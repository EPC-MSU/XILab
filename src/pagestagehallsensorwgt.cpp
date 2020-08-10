#include <pagestagehallsensorwgt.h>
#include <ui_pagestagehallsensorwgt.h>
#include <float.h>
#include <functions.h>

PageStageHallsensorWgt::PageStageHallsensorWgt(QWidget *parent, StageSettings *_stageStgs) :
    QWidget(parent),
    m_ui(new Ui::PageStageHallsensorWgtClass)
{
    m_ui->setupUi(this);
	stageStgs = _stageStgs;

#ifndef SERVICEMODE
	m_ui->manufacturerEdit->setReadOnly(true);
	m_ui->partnumberEdit->setReadOnly(true);

	m_ui->maxOperatingFrequencySpin->setReadOnly(true);
	m_ui->supplyVoltageMinSpin->setReadOnly(true);
	m_ui->supplyVoltageMaxSpin->setReadOnly(true);
	m_ui->maxCurrentConsumptionSpin->setReadOnly(true);
	m_ui->PPRSpin->setReadOnly(true);
#endif
}

PageStageHallsensorWgt::~PageStageHallsensorWgt()
{
    delete m_ui;
}

void PageStageHallsensorWgt::SetDisabled(bool set)
{
	m_ui->manufacturerEdit->setDisabled(set);
	m_ui->partnumberEdit->setDisabled(set);

	m_ui->maxOperatingFrequencySpin->setDisabled(set);
	m_ui->supplyVoltageMinSpin->setDisabled(set);
	m_ui->supplyVoltageMaxSpin->setDisabled(set);
	m_ui->maxCurrentConsumptionSpin->setDisabled(set);
	m_ui->PPRSpin->setDisabled(set);

	if (set == true) {
		m_ui->manufacturerEdit->setText("");
		m_ui->partnumberEdit->setText("");

		m_ui->maxOperatingFrequencySpin->setValue(0);
		m_ui->supplyVoltageMinSpin->setValue(0);
		m_ui->supplyVoltageMaxSpin->setValue(0);
		m_ui->maxCurrentConsumptionSpin->setValue(0);
		m_ui->PPRSpin->setValue(0);
	}
}

void PageStageHallsensorWgt::FromClassToUi()
{
	m_ui->manufacturerEdit->setText(QString::fromUtf8(stageStgs->hallsensor_information.Manufacturer));
	m_ui->partnumberEdit->setText(QString::fromUtf8(stageStgs->hallsensor_information.PartNumber));

	m_ui->maxOperatingFrequencySpin->setValue(stageStgs->hallsensor_settings.MaxOperatingFrequency);
	m_ui->supplyVoltageMinSpin->setValue(stageStgs->hallsensor_settings.SupplyVoltageMin);
	m_ui->supplyVoltageMaxSpin->setValue(stageStgs->hallsensor_settings.SupplyVoltageMax);
	m_ui->maxCurrentConsumptionSpin->setValue(stageStgs->hallsensor_settings.MaxCurrentConsumption);
	m_ui->PPRSpin->setValue(stageStgs->hallsensor_settings.PPR);
}

void PageStageHallsensorWgt::FromUiToClass(StageSettings *lsStgs)
{
	safe_copy(lsStgs->hallsensor_information.Manufacturer, m_ui->manufacturerEdit->text().toUtf8().data());
	safe_copy(lsStgs->hallsensor_information.PartNumber, m_ui->partnumberEdit->text().toUtf8().data());

	lsStgs->hallsensor_settings.MaxOperatingFrequency = m_ui->maxOperatingFrequencySpin->value();
	lsStgs->hallsensor_settings.SupplyVoltageMin = m_ui->supplyVoltageMinSpin->value();
	lsStgs->hallsensor_settings.SupplyVoltageMax = m_ui->supplyVoltageMaxSpin->value();
	lsStgs->hallsensor_settings.MaxCurrentConsumption = m_ui->maxCurrentConsumptionSpin->value();
	lsStgs->hallsensor_settings.PPR = m_ui->PPRSpin->value();
}
