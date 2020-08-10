#include <QFontMetrics>
#include <pagestagewgt.h>
#include <ui_pagestagewgt.h>
#include <float.h>
#include <functions.h>

PageStageWgt::PageStageWgt(QWidget *parent, StageSettings *_stageStgs) :
    QWidget(parent),
    m_ui(new Ui::PageStageWgtClass)
{
    m_ui->setupUi(this);
	stageStgs = _stageStgs;

#ifndef SERVICEMODE
	m_ui->manufacturerEdit->setReadOnly(true);
	m_ui->partnumberEdit->setReadOnly(true);

	m_ui->leadScrewPitchSpin->setReadOnly(true);
	m_ui->unitsEdit->setReadOnly(true);
	m_ui->maxSpeedSpin->setReadOnly(true);
	m_ui->travelRangeSpin->setReadOnly(true);
	m_ui->supplyVoltageMinSpin->setReadOnly(true);
	m_ui->supplyVoltageMaxSpin->setReadOnly(true);
	m_ui->maxCurrentConsumptionSpin->setReadOnly(true);
	m_ui->horizontalLoadCapacitySpin->setReadOnly(true);
	m_ui->verticalLoadCapacitySpin->setReadOnly(true);
#endif

	testValues();

	connect(m_ui->unitsEdit, SIGNAL(textChanged(QString)), this, SLOT(testValues()));
}

PageStageWgt::~PageStageWgt()
{
    delete m_ui;
}

void PageStageWgt::SetDisabled(bool set)
{
	m_ui->manufacturerEdit->setDisabled(set);
	m_ui->partnumberEdit->setDisabled(set);
	m_ui->unitsEdit->setDisabled(set);

	m_ui->leadScrewPitchSpin->setDisabled(set);
	m_ui->maxSpeedSpin->setDisabled(set);
	m_ui->travelRangeSpin->setDisabled(set);
	m_ui->supplyVoltageMinSpin->setDisabled(set);
	m_ui->supplyVoltageMaxSpin->setDisabled(set);
	m_ui->maxCurrentConsumptionSpin->setDisabled(set);
	m_ui->horizontalLoadCapacitySpin->setDisabled(set);
	m_ui->verticalLoadCapacitySpin->setDisabled(set);

	if (set == true) {
		m_ui->manufacturerEdit->setText("");
		m_ui->partnumberEdit->setText("");
		m_ui->unitsEdit->setText("");

		m_ui->leadScrewPitchSpin->setValue(0);
		m_ui->maxSpeedSpin->setValue(0);
		m_ui->travelRangeSpin->setValue(0);
		m_ui->supplyVoltageMinSpin->setValue(0);
		m_ui->supplyVoltageMaxSpin->setValue(0);
		m_ui->maxCurrentConsumptionSpin->setValue(0);
		m_ui->horizontalLoadCapacitySpin->setValue(0);
		m_ui->verticalLoadCapacitySpin->setValue(0);
	}
}

void PageStageWgt::testValues()
{
	m_ui->unitsEdit->blockSignals(true);

	QString units = m_ui->unitsEdit->text();
	if(units.trimmed().isEmpty()){
		units = "";
		m_ui->unitsEdit->setText(units);
		m_ui->maxSpeedSpin->setSuffix("");
	}
	else
		m_ui->maxSpeedSpin->setSuffix(" "+units+"/s");

	m_ui->travelRangeSpin->setSuffix(" "+units);
	m_ui->leadScrewPitchSpin->setSuffix(" "+units);

	m_ui->unitsEdit->blockSignals(false);
}


void PageStageWgt::FromClassToUi()
{
	m_ui->manufacturerEdit->setText(QString::fromUtf8(stageStgs->stage_information.Manufacturer));
	m_ui->partnumberEdit->setText(QString::fromUtf8(stageStgs->stage_information.PartNumber));

	m_ui->unitsEdit->setText(QString::fromUtf8(stageStgs->stage_settings.Units));
	// everyone who uses unit text should be below it so that testValues function has already run on then
	m_ui->leadScrewPitchSpin->setValue(stageStgs->stage_settings.LeadScrewPitch);
	m_ui->maxSpeedSpin->setValue(stageStgs->stage_settings.MaxSpeed);
	m_ui->travelRangeSpin->setValue(stageStgs->stage_settings.TravelRange);
	m_ui->supplyVoltageMinSpin->setValue(stageStgs->stage_settings.SupplyVoltageMin);
	m_ui->supplyVoltageMaxSpin->setValue(stageStgs->stage_settings.SupplyVoltageMax);
	m_ui->maxCurrentConsumptionSpin->setValue(stageStgs->stage_settings.MaxCurrentConsumption);
	m_ui->horizontalLoadCapacitySpin->setValue(stageStgs->stage_settings.HorizontalLoadCapacity);
	m_ui->verticalLoadCapacitySpin->setValue(stageStgs->stage_settings.VerticalLoadCapacity);
}

void PageStageWgt::FromUiToClass(StageSettings *lsStgs)
{
	safe_copy(lsStgs->stage_information.Manufacturer, m_ui->manufacturerEdit->text().toUtf8().data());
	safe_copy(lsStgs->stage_information.PartNumber, m_ui->partnumberEdit->text().toUtf8().data());

	lsStgs->stage_settings.LeadScrewPitch = m_ui->leadScrewPitchSpin->value();
	safe_copy(lsStgs->stage_settings.Units, m_ui->unitsEdit->text().toUtf8().data());
	lsStgs->stage_settings.MaxSpeed = m_ui->maxSpeedSpin->value();
	lsStgs->stage_settings.TravelRange = m_ui->travelRangeSpin->value();
	lsStgs->stage_settings.SupplyVoltageMin = m_ui->supplyVoltageMinSpin->value();
	lsStgs->stage_settings.SupplyVoltageMax = m_ui->supplyVoltageMaxSpin->value();
	lsStgs->stage_settings.MaxCurrentConsumption = m_ui->maxCurrentConsumptionSpin->value();
	lsStgs->stage_settings.HorizontalLoadCapacity = m_ui->horizontalLoadCapacitySpin->value();
	lsStgs->stage_settings.VerticalLoadCapacity = m_ui->verticalLoadCapacitySpin->value();
}
