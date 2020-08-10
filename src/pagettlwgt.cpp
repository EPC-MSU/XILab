#include <pagettlwgt.h>
#include <ui_pagettlwgt.h>
#include <functions.h>

PageTtlWgt::PageTtlWgt(QWidget *parent, MotorSettings *motorStgs) :
    QWidget(parent),
	m_ui(new Ui::PageTtlWgtClass)
{
    m_ui->setupUi(this);
	mStgs = motorStgs;

	m_ui->accuracyBox->setUnitType(UserUnitSettings::TYPE_COORD);
	m_ui->positionBox->setUnitType(UserUnitSettings::TYPE_COORD);
	m_ui->speedBox->setUnitType(UserUnitSettings::TYPE_SPEED);

	QObject::connect(m_ui->SyncoutEnabled, SIGNAL(toggled(bool)),	this,	SLOT(OnOneOfTwoChecked()));
	QObject::connect(m_ui->SyncoutOnperiod, SIGNAL(toggled(bool)),	this,	SLOT(OnOneOfTwoChecked()));

	m_ui->SyncinEnabled->toggle();
	m_ui->SyncoutEnabled->toggle();
	m_ui->frame->setFrameShape(QFrame::NoFrame);

	QObject::connect(m_ui->SyncoutEnabled, SIGNAL(toggled(bool)), m_ui->accuracyBox, SLOT(setEnabled(bool)));
}

PageTtlWgt::~PageTtlWgt()
{
	delete m_ui;
}

void PageTtlWgt::RewriteClutterValue()
{
	m_ui->clutterBox->setValue(((int)m_ui->clutterBox->value()/1000)*1000);
}

void PageTtlWgt::OnOneOfTwoChecked()
{
	m_ui->periodBox->setEnabled(m_ui->SyncoutEnabled->isChecked() && m_ui->SyncoutOnperiod->isChecked());
}

void PageTtlWgt::FromClassToUi()
{
	m_ui->clutterBox->setValue(mStgs->sync_in.ClutterTime);
	m_ui->positionBox->setBaseValue(mStgs->sync_in.Position, mStgs->sync_in.uPosition);
	m_ui->speedBox->setBaseValue(mStgs->sync_in.Speed, mStgs->sync_in.uSpeed);

	//sync_in flags
	m_ui->SyncinEnabled->setChecked(mStgs->sync_in.SyncInFlags & SYNCIN_ENABLED);
	m_ui->SyncinInvert->setChecked(mStgs->sync_in.SyncInFlags & SYNCIN_INVERT);
	m_ui->SyncinGotoposition->setChecked(mStgs->sync_in.SyncInFlags & SYNCIN_GOTOPOSITION);

	//syncout flags
	m_ui->SyncoutEnabled->setChecked(mStgs->sync_out.SyncOutFlags & SYNCOUT_ENABLED);
	m_ui->SyncoutState_0->setChecked((mStgs->sync_out.SyncOutFlags & SYNCOUT_STATE) == 0);
	m_ui->SyncoutState_1->setChecked((mStgs->sync_out.SyncOutFlags & SYNCOUT_STATE) != 0);
	m_ui->SyncoutInvert->setChecked(mStgs->sync_out.SyncOutFlags & SYNCOUT_INVERT);
	m_ui->impulseUnitsLst->setCurrentIndex((mStgs->sync_out.SyncOutFlags & SYNCOUT_IN_STEPS) != 0);
	m_ui->SyncoutOnstart->setChecked(mStgs->sync_out.SyncOutFlags & SYNCOUT_ONSTART);
	m_ui->SyncoutOnstop->setChecked(mStgs->sync_out.SyncOutFlags & SYNCOUT_ONSTOP);
	m_ui->SyncoutOnperiod->setChecked(mStgs->sync_out.SyncOutFlags & SYNCOUT_ONPERIOD);

	m_ui->pulseBox->setValue(mStgs->sync_out.SyncOutPulseSteps);
	m_ui->periodBox->setValue(mStgs->sync_out.SyncOutPeriod);
	m_ui->accuracyBox->setBaseValue(mStgs->sync_out.Accuracy, mStgs->sync_out.uAccuracy);
}

void PageTtlWgt::FromUiToClass(MotorSettings *lmStgs)
{
	lmStgs->sync_in.SyncInFlags = 0;
	lmStgs->sync_out.SyncOutFlags = 0;

	lmStgs->sync_in.ClutterTime = m_ui->clutterBox->value();
	lmStgs->sync_in.Position = m_ui->positionBox->getBaseStep();
	lmStgs->sync_in.uPosition = m_ui->positionBox->getBaseUStep();
	lmStgs->sync_in.Speed = m_ui->speedBox->UgetBaseStep();
	lmStgs->sync_in.uSpeed = m_ui->speedBox->UgetBaseUStep();

	//syncin flags
	setUnsetBit(m_ui->SyncinEnabled->isChecked(), &lmStgs->sync_in.SyncInFlags, SYNCIN_ENABLED);
	setUnsetBit(m_ui->SyncinInvert->isChecked(), &lmStgs->sync_in.SyncInFlags, SYNCIN_INVERT);
	setUnsetBit(m_ui->SyncinGotoposition->isChecked(), &lmStgs->sync_in.SyncInFlags, SYNCIN_GOTOPOSITION);

	//syncout flags
	setUnsetBit(m_ui->SyncoutEnabled->isChecked(), &lmStgs->sync_out.SyncOutFlags, SYNCOUT_ENABLED);
	setUnsetBit(m_ui->SyncoutState_1->isChecked(), &lmStgs->sync_out.SyncOutFlags, SYNCOUT_STATE);
	setUnsetBit(m_ui->SyncoutInvert->isChecked(), &lmStgs->sync_out.SyncOutFlags, SYNCOUT_INVERT);
	setUnsetBit((m_ui->impulseUnitsLst->currentIndex() == 1), &lmStgs->sync_out.SyncOutFlags, SYNCOUT_IN_STEPS);
	setUnsetBit(m_ui->SyncoutOnstart->isChecked(), &lmStgs->sync_out.SyncOutFlags, SYNCOUT_ONSTART);
	setUnsetBit(m_ui->SyncoutOnstop->isChecked(), &lmStgs->sync_out.SyncOutFlags, SYNCOUT_ONSTOP);
	setUnsetBit(m_ui->SyncoutOnperiod->isChecked(), &lmStgs->sync_out.SyncOutFlags, SYNCOUT_ONPERIOD);

	lmStgs->sync_out.SyncOutPulseSteps = m_ui->pulseBox->value();
	lmStgs->sync_out.SyncOutPeriod = m_ui->periodBox->value();
	lmStgs->sync_out.Accuracy = m_ui->accuracyBox->UgetBaseStep();
	lmStgs->sync_out.uAccuracy = m_ui->accuracyBox->UgetBaseUStep();
}
