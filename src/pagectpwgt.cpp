#include <pagectpwgt.h>
#include <ui_pagectpwgt.h>

PageControlPositionWgt::PageControlPositionWgt(QWidget *parent, MotorSettings *motorStgs) :
    QWidget(parent),
    m_ui(new Ui::PageControlPositionWgtClass)
{
	m_ui->setupUi(this);
	mStgs = motorStgs;
}

PageControlPositionWgt::~PageControlPositionWgt()
{
    delete m_ui;
}

void PageControlPositionWgt::FromClassToUi() // From MotorSettings to UI
{
	m_ui->PositionControlChk->setChecked(mStgs->ctp.CTPFlags & CTP_ENABLED);
	if (mStgs->ctp.CTPFlags & CTP_ALARM_ON_ERROR)
		m_ui->alarmRadioButton->setChecked(true);
	else if (mStgs->ctp.CTPFlags & CTP_ERROR_CORRECTION)
		m_ui->correctRadioButton->setChecked(true);
	else
		m_ui->ignoreRadioButton->setChecked(true);
	m_ui->revSensInv->setChecked(mStgs->ctp.CTPFlags & REV_SENS_INV);
	m_ui->minErrorSpin->setValue(mStgs->ctp.CTPMinError);

	if(mStgs->ctp.CTPFlags & CTP_BASE)
		m_ui->baseCombo->setCurrentIndex(0);
	else
		m_ui->baseCombo->setCurrentIndex(1);
}

void PageControlPositionWgt::FromUiToClass(MotorSettings *lmStgs)
{
	lmStgs->ctp.CTPFlags = 0;

	setUnsetBit(m_ui->PositionControlChk->isChecked(), &lmStgs->ctp.CTPFlags, CTP_ENABLED);
	setUnsetBit(m_ui->alarmRadioButton->isChecked(), &lmStgs->ctp.CTPFlags, CTP_ALARM_ON_ERROR);
	setUnsetBit(m_ui->correctRadioButton->isChecked(), &lmStgs->ctp.CTPFlags, CTP_ERROR_CORRECTION);

	setUnsetBit(m_ui->revSensInv->isChecked(), &lmStgs->ctp.CTPFlags, REV_SENS_INV);
	setUnsetBit((m_ui->baseCombo->currentIndex() == 0), &lmStgs->ctp.CTPFlags, CTP_BASE);

	lmStgs->ctp.CTPMinError = m_ui->minErrorSpin->value();
}

void PageControlPositionWgt::setPartEnabled(bool enable) {
	m_ui->PositionControlChk->setEnabled(enable);
}
