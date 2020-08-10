#include <pagepowerwgt.h>
#include <ui_pagepowerwgt.h>

PagePowerWgt::PagePowerWgt(QWidget *parent, MotorSettings *motorStgs) :
    QWidget(parent),
    m_ui(new Ui::PagePowerWgtClass)
{
	m_ui->setupUi(this);
	mStgs = motorStgs;
}

PagePowerWgt::~PagePowerWgt()
{
    delete m_ui;
}

void PagePowerWgt::FromClassToUi()
{
	m_ui->holdCurrentSpin->setValue(mStgs->power.HoldCurrent);
	m_ui->currReductDelaySpin->setValue(mStgs->power.CurrReductDelay);
	m_ui->powerOffDelaySpin->setValue(mStgs->power.PowerOffDelay);
	m_ui->smoothCurrentSpin->setValue(mStgs->power.CurrentSetTime);

	m_ui->powerOffChk->setChecked(mStgs->power.PowerFlags & POWER_OFF_ENABLED);
	m_ui->currentReductChk->setChecked(mStgs->power.PowerFlags & POWER_REDUCT_ENABLED);
	m_ui->smoothCurrentChk->setChecked(mStgs->power.PowerFlags & POWER_SMOOTH_CURRENT);
}

void PagePowerWgt::FromUiToClass(MotorSettings *lmStgs)
{
	lmStgs->power.PowerFlags = 0;

	lmStgs->power.HoldCurrent = m_ui->holdCurrentSpin->value();
	lmStgs->power.CurrReductDelay = m_ui->currReductDelaySpin->value();
	lmStgs->power.PowerOffDelay = m_ui->powerOffDelaySpin->value();
	lmStgs->power.CurrentSetTime = m_ui->smoothCurrentSpin->value();
	setUnsetBit(m_ui->powerOffChk->isChecked(), &lmStgs->power.PowerFlags, POWER_OFF_ENABLED);
	setUnsetBit(m_ui->currentReductChk->isChecked(), &lmStgs->power.PowerFlags, POWER_REDUCT_ENABLED);
	setUnsetBit(m_ui->smoothCurrentChk->isChecked(), &lmStgs->power.PowerFlags, POWER_SMOOTH_CURRENT);
}
