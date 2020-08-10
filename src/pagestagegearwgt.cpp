#include <pagestagegearwgt.h>
#include <ui_pagestagegearwgt.h>
#include <float.h>
#include <functions.h>

PageStageGearWgt::PageStageGearWgt(QWidget *parent, StageSettings *_stageStgs) :
    QWidget(parent),
    m_ui(new Ui::PageStageGearWgtClass)
{
    m_ui->setupUi(this);
	stageStgs = _stageStgs;

#ifndef SERVICEMODE
	m_ui->manufacturerEdit->setReadOnly(true);
	m_ui->partnumberEdit->setReadOnly(true);

	m_ui->reductionInSpin->setReadOnly(true);
	m_ui->reductionOutSpin->setReadOnly(true);
	m_ui->ratedInputTorqueSpin->setReadOnly(true);
	m_ui->ratedInputSpeedSpin->setReadOnly(true);
	m_ui->maxOutputBacklashSpin->setReadOnly(true);
	m_ui->inputInertiaSpin->setReadOnly(true);
	m_ui->efficiencySpin->setReadOnly(true);
#endif
}

PageStageGearWgt::~PageStageGearWgt()
{
    delete m_ui;
}

void PageStageGearWgt::SetDisabled(bool set)
{
	m_ui->manufacturerEdit->setDisabled(set);
	m_ui->partnumberEdit->setDisabled(set);

	m_ui->reductionInSpin->setDisabled(set);
	m_ui->reductionOutSpin->setDisabled(set);
	m_ui->ratedInputTorqueSpin->setDisabled(set);
	m_ui->ratedInputSpeedSpin->setDisabled(set);
	m_ui->maxOutputBacklashSpin->setDisabled(set);
	m_ui->inputInertiaSpin->setDisabled(set);
	m_ui->efficiencySpin->setDisabled(set);

	if (set == true) {
		m_ui->manufacturerEdit->setText("");
		m_ui->partnumberEdit->setText("");

		m_ui->reductionInSpin->setValue(0);
		m_ui->reductionOutSpin->setValue(0);
		m_ui->ratedInputTorqueSpin->setValue(0);
		m_ui->ratedInputSpeedSpin->setValue(0);
		m_ui->maxOutputBacklashSpin->setValue(0);
		m_ui->inputInertiaSpin->setValue(0);
		m_ui->efficiencySpin->setValue(0);
	}
}

void PageStageGearWgt::FromClassToUi()
{
	m_ui->manufacturerEdit->setText(QString::fromUtf8(stageStgs->gear_information.Manufacturer));
	m_ui->partnumberEdit->setText(QString::fromUtf8(stageStgs->gear_information.PartNumber));

	m_ui->reductionInSpin->setValue(stageStgs->gear_settings.ReductionIn);
	m_ui->reductionOutSpin->setValue(stageStgs->gear_settings.ReductionOut);
	m_ui->ratedInputTorqueSpin->setValue(stageStgs->gear_settings.RatedInputTorque);
	m_ui->ratedInputSpeedSpin->setValue(stageStgs->gear_settings.RatedInputSpeed);
	m_ui->maxOutputBacklashSpin->setValue(stageStgs->gear_settings.MaxOutputBacklash);
	m_ui->inputInertiaSpin->setValue(stageStgs->gear_settings.InputInertia);
	m_ui->efficiencySpin->setValue(stageStgs->gear_settings.Efficiency);
}

void PageStageGearWgt::FromUiToClass(StageSettings *lsStgs)
{
	safe_copy(lsStgs->gear_information.Manufacturer, m_ui->manufacturerEdit->text().toUtf8().data());
	safe_copy(lsStgs->gear_information.PartNumber, m_ui->partnumberEdit->text().toUtf8().data());

	lsStgs->gear_settings.ReductionIn = m_ui->reductionInSpin->value();
	lsStgs->gear_settings.ReductionOut = m_ui->reductionOutSpin->value();
	lsStgs->gear_settings.RatedInputTorque = m_ui->ratedInputTorqueSpin->value();
	lsStgs->gear_settings.RatedInputSpeed = m_ui->ratedInputSpeedSpin->value();
	lsStgs->gear_settings.MaxOutputBacklash = m_ui->maxOutputBacklashSpin->value();
	lsStgs->gear_settings.InputInertia = m_ui->inputInertiaSpin->value();
	lsStgs->gear_settings.Efficiency = m_ui->efficiencySpin->value();
}
