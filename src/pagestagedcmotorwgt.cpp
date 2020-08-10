#include <pagestagedcmotorwgt.h>
#include <ui_pagestagedcmotorwgt.h>
#include <float.h>
#include <functions.h>

PageStageDCMotorWgt::PageStageDCMotorWgt(QWidget *parent, StageSettings *_stageStgs) :
    QWidget(parent),
    m_ui(new Ui::PageStageDCMotorWgtClass)
{
    m_ui->setupUi(this);
	stageStgs = _stageStgs;

	QObject::connect(m_ui->motorTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(OnMotorTypeChanged(int)));

#ifndef SERVICEMODE
	m_ui->manufacturerEdit->setReadOnly(true);
	m_ui->partnumberEdit->setReadOnly(true);

//	m_ui->motorTypeCombo->setReadOnly(true);
	m_ui->polesSpin->setReadOnly(true);
	m_ui->phasesSpin->setReadOnly(true);
	m_ui->nominalVoltageSpin->setReadOnly(true);
	m_ui->nominalCurrentSpin->setReadOnly(true);
	m_ui->nominalSpeedSpin->setReadOnly(true);
	m_ui->nominalTorqueSpin->setReadOnly(true);
	m_ui->nominalPowerSpin->setReadOnly(true);
	m_ui->windingResistanceSpin->setReadOnly(true);
	m_ui->windingInductanceSpin->setReadOnly(true);
	m_ui->rotorInertiaSpin->setReadOnly(true);
	m_ui->stallTorqueSpin->setReadOnly(true);
	m_ui->detentTorqueSpin->setReadOnly(true);
	m_ui->torqueConstantSpin->setReadOnly(true);
	m_ui->speedConstantSpin->setReadOnly(true);
	m_ui->speedTorqueGradientSpin->setReadOnly(true);
	m_ui->mechanicalTimeConstantSpin->setReadOnly(true);
	m_ui->maxSpeedSpin->setReadOnly(true);
	m_ui->maxCurrentSpin->setReadOnly(true);
	m_ui->maxCurrentTimeSpin->setReadOnly(true);
	m_ui->noLoadCurrentSpin->setReadOnly(true);
	m_ui->noLoadSpeedSpin->setReadOnly(true);
#endif
}

PageStageDCMotorWgt::~PageStageDCMotorWgt()
{
    delete m_ui;
}

void PageStageDCMotorWgt::SetDisabled(bool set)
{
	m_ui->manufacturerEdit->setDisabled(set);
	m_ui->partnumberEdit->setDisabled(set);

	m_ui->motorTypeCombo->setDisabled(set);
	m_ui->polesSpin->setDisabled(set);
	m_ui->phasesSpin->setDisabled(set);
	m_ui->nominalVoltageSpin->setDisabled(set);
	m_ui->nominalCurrentSpin->setDisabled(set);
	m_ui->nominalSpeedSpin->setDisabled(set);
	m_ui->nominalTorqueSpin->setDisabled(set);
	m_ui->nominalPowerSpin->setDisabled(set);
	m_ui->windingResistanceSpin->setDisabled(set);
	m_ui->windingInductanceSpin->setDisabled(set);
	m_ui->rotorInertiaSpin->setDisabled(set);
	m_ui->stallTorqueSpin->setDisabled(set);
	m_ui->detentTorqueSpin->setDisabled(set);
	m_ui->torqueConstantSpin->setDisabled(set);
	m_ui->speedConstantSpin->setDisabled(set);
	m_ui->speedTorqueGradientSpin->setDisabled(set);
	m_ui->mechanicalTimeConstantSpin->setDisabled(set);
	m_ui->maxSpeedSpin->setDisabled(set);
	m_ui->maxCurrentSpin->setDisabled(set);
	m_ui->maxCurrentTimeSpin->setDisabled(set);
	m_ui->noLoadCurrentSpin->setDisabled(set);
	m_ui->noLoadSpeedSpin->setDisabled(set);
	OnMotorTypeChanged(m_ui->motorTypeCombo->currentIndex());

	if (set == true) {
		m_ui->manufacturerEdit->setText("");
		m_ui->partnumberEdit->setText("");

		m_ui->motorTypeCombo->setCurrentIndex(-1);
		m_ui->polesSpin->setValue(0);
		m_ui->phasesSpin->setValue(0);
		m_ui->nominalVoltageSpin->setValue(0);
		m_ui->nominalCurrentSpin->setValue(0);
		m_ui->nominalSpeedSpin->setValue(0);
		m_ui->nominalTorqueSpin->setValue(0);
		m_ui->nominalPowerSpin->setValue(0);
		m_ui->windingResistanceSpin->setValue(0);
		m_ui->windingInductanceSpin->setValue(0);
		m_ui->rotorInertiaSpin->setValue(0);
		m_ui->stallTorqueSpin->setValue(0);
		m_ui->detentTorqueSpin->setValue(0);
		m_ui->torqueConstantSpin->setValue(0);
		m_ui->speedConstantSpin->setValue(0);
		m_ui->speedTorqueGradientSpin->setValue(0);
		m_ui->mechanicalTimeConstantSpin->setValue(0);
		m_ui->maxSpeedSpin->setValue(0);
		m_ui->maxCurrentSpin->setValue(0);
		m_ui->maxCurrentTimeSpin->setValue(0);
		m_ui->noLoadCurrentSpin->setValue(0);
		m_ui->noLoadSpeedSpin->setValue(0);
	}
}

void PageStageDCMotorWgt::FromClassToUi()
{
	m_ui->manufacturerEdit->setText(QString::fromUtf8(stageStgs->motor_information.Manufacturer));
	m_ui->partnumberEdit->setText(QString::fromUtf8(stageStgs->motor_information.PartNumber));

	int selected = stageStgs->motor_settings.MotorType-1;
	m_ui->motorTypeCombo->setCurrentIndex(selected); // motor type 0x01 mapped to index 0
#ifndef SERVICEMODE
	ComboboxGrayout(m_ui->motorTypeCombo, selected);
#endif

	m_ui->polesSpin->setValue(stageStgs->motor_settings.Poles);
	m_ui->phasesSpin->setValue(stageStgs->motor_settings.Phases);
	m_ui->nominalVoltageSpin->setValue(stageStgs->motor_settings.NominalVoltage);
	m_ui->nominalCurrentSpin->setValue(stageStgs->motor_settings.NominalCurrent);
	m_ui->nominalSpeedSpin->setValue(stageStgs->motor_settings.NominalSpeed);
	m_ui->nominalTorqueSpin->setValue(stageStgs->motor_settings.NominalTorque);
	m_ui->nominalPowerSpin->setValue(stageStgs->motor_settings.NominalPower);
	m_ui->windingResistanceSpin->setValue(stageStgs->motor_settings.WindingResistance);
	m_ui->windingInductanceSpin->setValue(stageStgs->motor_settings.WindingInductance);
	m_ui->rotorInertiaSpin->setValue(stageStgs->motor_settings.RotorInertia);
	m_ui->stallTorqueSpin->setValue(stageStgs->motor_settings.StallTorque);
	m_ui->detentTorqueSpin->setValue(stageStgs->motor_settings.DetentTorque);
	m_ui->torqueConstantSpin->setValue(stageStgs->motor_settings.TorqueConstant);
	m_ui->speedConstantSpin->setValue(stageStgs->motor_settings.SpeedConstant);
	m_ui->speedTorqueGradientSpin->setValue(stageStgs->motor_settings.SpeedTorqueGradient);
	m_ui->mechanicalTimeConstantSpin->setValue(stageStgs->motor_settings.MechanicalTimeConstant);
	m_ui->maxSpeedSpin->setValue(stageStgs->motor_settings.MaxSpeed);
	m_ui->maxCurrentSpin->setValue(stageStgs->motor_settings.MaxCurrent);
	m_ui->maxCurrentTimeSpin->setValue(stageStgs->motor_settings.MaxCurrentTime);
	m_ui->noLoadCurrentSpin->setValue(stageStgs->motor_settings.NoLoadCurrent);
	m_ui->noLoadSpeedSpin->setValue(stageStgs->motor_settings.NoLoadSpeed);
}

void PageStageDCMotorWgt::FromUiToClass(StageSettings *lsStgs)
{
	safe_copy(lsStgs->motor_information.Manufacturer, m_ui->manufacturerEdit->text().toUtf8().data());
	safe_copy(lsStgs->motor_information.PartNumber, m_ui->partnumberEdit->text().toUtf8().data());
	
	lsStgs->motor_settings.MotorType = m_ui->motorTypeCombo->currentIndex()+1; // index 0 mapped to motor type 0x01
	lsStgs->motor_settings.Poles = m_ui->polesSpin->value();
	lsStgs->motor_settings.Phases = m_ui->phasesSpin->value();
	lsStgs->motor_settings.NominalVoltage = m_ui->nominalVoltageSpin->value();
	lsStgs->motor_settings.NominalCurrent = m_ui->nominalCurrentSpin->value();
	lsStgs->motor_settings.NominalSpeed = m_ui->nominalSpeedSpin->value();
	lsStgs->motor_settings.NominalTorque = m_ui->nominalTorqueSpin->value();
	lsStgs->motor_settings.NominalPower = m_ui->nominalPowerSpin->value();
	lsStgs->motor_settings.WindingResistance = m_ui->windingResistanceSpin->value();
	lsStgs->motor_settings.WindingInductance = m_ui->windingInductanceSpin->value();
	lsStgs->motor_settings.RotorInertia = m_ui->rotorInertiaSpin->value();
	lsStgs->motor_settings.StallTorque = m_ui->stallTorqueSpin->value();
	lsStgs->motor_settings.DetentTorque = m_ui->detentTorqueSpin->value();
	lsStgs->motor_settings.TorqueConstant = m_ui->torqueConstantSpin->value();
	lsStgs->motor_settings.SpeedConstant = m_ui->speedConstantSpin->value();
	lsStgs->motor_settings.SpeedTorqueGradient = m_ui->speedTorqueGradientSpin->value();
	lsStgs->motor_settings.MechanicalTimeConstant = m_ui->mechanicalTimeConstantSpin->value();
	lsStgs->motor_settings.MaxSpeed = m_ui->maxSpeedSpin->value();
	lsStgs->motor_settings.MaxCurrent = m_ui->maxCurrentSpin->value();
	lsStgs->motor_settings.MaxCurrentTime = m_ui->maxCurrentTimeSpin->value();
	lsStgs->motor_settings.NoLoadCurrent = m_ui->noLoadCurrentSpin->value();
	lsStgs->motor_settings.NoLoadSpeed = m_ui->noLoadSpeedSpin->value();	
}

void PageStageDCMotorWgt::OnMotorTypeChanged(int index)
{
#ifndef SERVICEMODE
	m_ui->motorTypeCombo->setCurrentIndex(stageStgs->motor_settings.MotorType - 1);
	index = m_ui->motorTypeCombo->currentIndex();
#endif

	QString speed_unit;
	bool phases = true, stepper = false; // initialize to show everything for unknown motor type
	if (index == 0) { //MOTOR_TYPE_STEP
		phases = false;
		stepper = true;
		speed_unit = "steps/s";
	} else if (index == 1) { // MOTOR_TYPE_DC
		phases = false;
		stepper = false;
		speed_unit = "rpm";
	} else if (index == 2) { // MOTOR_TYPE_BLDC by default
		phases = true;
		stepper = false;
		speed_unit = "rpm";
	}

	bool readonly = !m_ui->manufacturerEdit->isEnabled(); // 
	m_ui->phasesSpin->setDisabled(!phases || readonly);
	m_ui->nominalSpeedSpin->setDisabled(stepper || readonly);
	m_ui->nominalTorqueSpin->setDisabled(stepper || readonly);
	m_ui->nominalPowerSpin->setDisabled(stepper || readonly);
	m_ui->noLoadCurrentSpin->setDisabled(stepper || readonly);
	m_ui->noLoadSpeedSpin->setDisabled(stepper || readonly);

	m_ui->speedConstantSpin->setSuffix(" "+speed_unit+" / V");
	m_ui->maxSpeedSpin->setSuffix(" "+speed_unit);
}