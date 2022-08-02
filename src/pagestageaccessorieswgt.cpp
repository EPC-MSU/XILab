#include <pagestageaccessorieswgt.h>
#include <ui_pagestageaccessorieswgt.h>
#include <float.h>
#include <functions.h>

PageStageAccessoriesWgt::PageStageAccessoriesWgt(QWidget *parent, StageSettings *_stageStgs) :
    QWidget(parent),
    m_ui(new Ui::PageStageAccessoriesWgtClass)
{
    m_ui->setupUi(this);
	stageStgs = _stageStgs;

#ifndef SERVICEMODE
	QObject::connect(m_ui->tsTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(OnTSTypeChanged(int)));

	m_ui->magneticBrakeInfoEdit->setReadOnly(true);
	m_ui->mbRatedVoltageSpin->setReadOnly(true);
	m_ui->mbRatedCurrentSpin->setReadOnly(true);
	m_ui->mbTorqueSpin->setReadOnly(true);
	m_ui->mbAvailableCheck->setCheckable(false);
	m_ui->mbPoweredHoldCheck->setCheckable(false);

	m_ui->temperatureSensorInfoEdit->setReadOnly(true);
	m_ui->tsMinSpin->setReadOnly(true);
	m_ui->tsMaxSpin->setReadOnly(true);
	m_ui->tsGradSpin->setReadOnly(true);
//	m_ui->tsTypeCombo->setReadOnly(true);
	m_ui->tsAvailableCheck->setCheckable(false);

	m_ui->lsOnSw1AvailableCheck->setCheckable(false);
	m_ui->lsOnSw2AvailableCheck->setCheckable(false);
	m_ui->lsSw1ActiveLowCheck->setCheckable(false);
	m_ui->lsSw2ActiveLowCheck->setCheckable(false);
	m_ui->lsShortedCheck->setCheckable(false);
#endif
}

PageStageAccessoriesWgt::~PageStageAccessoriesWgt()
{
    delete m_ui;
}

void PageStageAccessoriesWgt::SetDisabled(bool set)
{
	m_ui->magneticBrakeInfoEdit->setDisabled(set);
	m_ui->mbRatedVoltageSpin->setDisabled(set);
	m_ui->mbRatedCurrentSpin->setDisabled(set);
	m_ui->mbTorqueSpin->setDisabled(set);
	m_ui->mbAvailableCheck->setDisabled(set);
	m_ui->mbPoweredHoldCheck->setDisabled(set);

	m_ui->temperatureSensorInfoEdit->setDisabled(set);
	m_ui->tsMinSpin->setDisabled(set);
	m_ui->tsMaxSpin->setDisabled(set);
	m_ui->tsGradSpin->setDisabled(set);
	m_ui->tsTypeCombo->setDisabled(set);
	m_ui->tsAvailableCheck->setDisabled(set);

	m_ui->lsOnSw1AvailableCheck->setDisabled(set);
	m_ui->lsOnSw2AvailableCheck->setDisabled(set);
	m_ui->lsSw1ActiveLowCheck->setDisabled(set);
	m_ui->lsSw2ActiveLowCheck->setDisabled(set);
	m_ui->lsShortedCheck->setDisabled(set);

	if (set == true) {
		m_ui->magneticBrakeInfoEdit->setText("");
		m_ui->mbRatedVoltageSpin->setValue(0);
		m_ui->mbRatedCurrentSpin->setValue(0);
		m_ui->mbTorqueSpin->setValue(0);
		m_ui->mbAvailableCheck->setChecked(false);
		m_ui->mbPoweredHoldCheck->setChecked(false);

		m_ui->temperatureSensorInfoEdit->setText("");
		m_ui->tsMinSpin->setValue(0);
		m_ui->tsMaxSpin->setValue(0);
		m_ui->tsGradSpin->setValue(0);
		m_ui->tsTypeCombo->setCurrentIndex(-1);
		m_ui->tsAvailableCheck->setChecked(false);

		m_ui->lsOnSw1AvailableCheck->setChecked(false);
		m_ui->lsOnSw2AvailableCheck->setChecked(false);
		m_ui->lsSw1ActiveLowCheck->setChecked(false);
		m_ui->lsSw2ActiveLowCheck->setChecked(false);
		m_ui->lsShortedCheck->setChecked(false);
	}

}

void PageStageAccessoriesWgt::FromClassToUi()
{
	m_ui->magneticBrakeInfoEdit->setText(QString::fromUtf8(stageStgs->accessories_settings.MagneticBrakeInfo));
	m_ui->mbRatedVoltageSpin->setValue(stageStgs->accessories_settings.MBRatedVoltage);
	m_ui->mbRatedCurrentSpin->setValue(stageStgs->accessories_settings.MBRatedCurrent);
	m_ui->mbTorqueSpin->setValue(stageStgs->accessories_settings.MBTorque);
	m_ui->mbAvailableCheck->setChecked(stageStgs->accessories_settings.MBSettings & MB_AVAILABLE);
	m_ui->mbPoweredHoldCheck->setChecked(stageStgs->accessories_settings.MBSettings & MB_POWERED_HOLD);

	m_ui->temperatureSensorInfoEdit->setText(QString::fromUtf8(stageStgs->accessories_settings.TemperatureSensorInfo));
	m_ui->tsMinSpin->setValue(stageStgs->accessories_settings.TSMin);
	m_ui->tsMaxSpin->setValue(stageStgs->accessories_settings.TSMax);
	m_ui->tsGradSpin->setValue(stageStgs->accessories_settings.TSGrad);

	int selected = stageStgs->accessories_settings.TSSettings & TS_TYPE_BITS;
	m_ui->tsTypeCombo->setCurrentIndex(selected-1);
#ifndef SERVICEMODE
	ComboboxGrayout(m_ui->tsTypeCombo, selected-1);
#endif
	m_ui->tsAvailableCheck->setChecked(stageStgs->accessories_settings.TSSettings & TS_AVAILABLE);

	m_ui->lsOnSw1AvailableCheck->setChecked(stageStgs->accessories_settings.LimitSwitchesSettings & LS_ON_SW1_AVAILABLE);
	m_ui->lsOnSw2AvailableCheck->setChecked(stageStgs->accessories_settings.LimitSwitchesSettings & LS_ON_SW2_AVAILABLE);
	m_ui->lsSw1ActiveLowCheck->setChecked(stageStgs->accessories_settings.LimitSwitchesSettings & LS_SW1_ACTIVE_LOW);
	m_ui->lsSw2ActiveLowCheck->setChecked(stageStgs->accessories_settings.LimitSwitchesSettings & LS_SW2_ACTIVE_LOW);
	m_ui->lsShortedCheck->setChecked(stageStgs->accessories_settings.LimitSwitchesSettings & LS_SHORTED);
}

void PageStageAccessoriesWgt::FromUiToClass(StageSettings *lsStgs)
{
	lsStgs->accessories_settings.MBSettings = 0;
	safe_copy(lsStgs->accessories_settings.MagneticBrakeInfo, m_ui->magneticBrakeInfoEdit->text().toUtf8().data());
	lsStgs->accessories_settings.MBRatedVoltage = m_ui->mbRatedVoltageSpin->value();
	lsStgs->accessories_settings.MBRatedCurrent = m_ui->mbRatedCurrentSpin->value();
	lsStgs->accessories_settings.MBTorque = m_ui->mbTorqueSpin->value();
	setUnsetBit(m_ui->mbAvailableCheck->isChecked(), &lsStgs->accessories_settings.MBSettings, MB_AVAILABLE);
	setUnsetBit(m_ui->mbPoweredHoldCheck->isChecked(), &lsStgs->accessories_settings.MBSettings, MB_POWERED_HOLD);

	lsStgs->accessories_settings.TSSettings = 0;
	safe_copy(lsStgs->accessories_settings.TemperatureSensorInfo, m_ui->temperatureSensorInfoEdit->text().toUtf8().data());
	lsStgs->accessories_settings.TSMin = m_ui->tsMinSpin->value();
	lsStgs->accessories_settings.TSMax = m_ui->tsMaxSpin->value();
	lsStgs->accessories_settings.TSGrad = m_ui->tsGradSpin->value();
	lsStgs->accessories_settings.TSSettings = m_ui->tsTypeCombo->currentIndex()+1;
	setUnsetBit(m_ui->tsAvailableCheck->isChecked(), &lsStgs->accessories_settings.TSSettings, TS_AVAILABLE);

	lsStgs->accessories_settings.LimitSwitchesSettings = 0;
	setUnsetBit(m_ui->lsOnSw1AvailableCheck->isChecked(), &lsStgs->accessories_settings.LimitSwitchesSettings, LS_ON_SW1_AVAILABLE);
	setUnsetBit(m_ui->lsOnSw2AvailableCheck->isChecked(), &lsStgs->accessories_settings.LimitSwitchesSettings, LS_ON_SW2_AVAILABLE);
	setUnsetBit(m_ui->lsSw1ActiveLowCheck->isChecked(), &lsStgs->accessories_settings.LimitSwitchesSettings, LS_SW1_ACTIVE_LOW);
	setUnsetBit(m_ui->lsSw2ActiveLowCheck->isChecked(), &lsStgs->accessories_settings.LimitSwitchesSettings, LS_SW2_ACTIVE_LOW);
	setUnsetBit(m_ui->lsShortedCheck->isChecked(), &lsStgs->accessories_settings.LimitSwitchesSettings, LS_SHORTED);
}

void PageStageAccessoriesWgt::OnTSTypeChanged(int index)
{
	Q_UNUSED(index)
	m_ui->tsTypeCombo->setCurrentIndex((stageStgs->accessories_settings.TSSettings & TS_TYPE_BITS) - 1);
}
