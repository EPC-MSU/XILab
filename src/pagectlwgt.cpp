#include <pagectlwgt.h>
#include <ui_pagectlwgt.h>
extern bool get_additional_enabled;

PageCTLWgt::PageCTLWgt(QWidget *parent, MotorSettings *motorStgs, UpdateThread *_updateThread) :
    QWidget(parent),
    m_ui(new Ui::PageCTLWgtClass)
{
	m_ui->setupUi(this);

	mStgs = motorStgs;
	updateThread = _updateThread;

	InitUI();
}

PageCTLWgt::~PageCTLWgt()
{
    delete m_ui;
}

void PageCTLWgt::InitUI()
{
	for(int i=0; i<9; i++){
		timeSpin[i].setSuffix(tr(" ms"));
		timeSpin[i].setMinimum(0);
		timeSpin[i].setMaximum(65535);
		timeSpin[i].setObjectName("timeSpin_" + toStr(i));
		m_ui->gridLayout->addWidget(&timeSpin[i], 2*i+2, 0, 2, 1);
	}
	for(int i=0; i<10; i++){
		m_ui->gridLayout->addWidget(&speedSpin[i], 2*i+1, 1, 2, 1);
		speedSpin[i].setObjectName("speedSpin_" + toStr(i));
		speedSpin[i].setUnitType(UserUnitSettings::TYPE_SPEED);
	}
	m_ui->deltaPositionBox->setUnitType(UserUnitSettings::TYPE_COORD);

	QObject::connect(m_ui->controlModeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(modeChanged(int)));
	QObject::connect(m_ui->joyCalibBtn,      SIGNAL(clicked()),                this, SLOT(OnJoyCalibBtnPressed()));
	modeChanged(m_ui->controlModeCombo->currentIndex());
}

void PageCTLWgt::OnJoyCalibBtnPressed()
{
	JoystickCalibrationDlg joyDlg(this, updateThread->devinterface, "");
	connect(&joyDlg,		SIGNAL(sendJoySettings(int, int, int, float)),	this,		SLOT(receiveJoySettings(int, int, int, float)));
	connect(updateThread,   SIGNAL(done()),									&joyDlg,    SLOT(RefreshWindow()));
	joyDlg.window()->layout()->setSizeConstraint( QLayout::SetFixedSize );

	bool saved_state = updateThread->devinterface->cs->additionalEnabled();
	updateThread->devinterface->cs->setAdditionalEnabled(true);
	joyDlg.exec();
	updateThread->devinterface->cs->setAdditionalEnabled(saved_state);
	
	disconnect(this,	SLOT(receiveJoySettings(int, int, int, float)));
	disconnect(&joyDlg,	SLOT(RefreshWindow()));
}

void PageCTLWgt::modeChanged(int i)
{
	m_ui->groupBox_2->setEnabled(i>0);
	m_ui->groupBox_3->setEnabled(i>0);
	m_ui->groupBox->setEnabled(i>0);
	m_ui->groupBox_4->setEnabled(i==1);
	for (int k=0; k<9; k++) {
		timeSpin[k].setEnabled(i==2);
	}
	m_ui->groupBox_5->setEnabled(i==2);
}

void PageCTLWgt::receiveJoySettings(int low, int center, int high, float deadzone)
{
	m_ui->joyLowEndSpin->setValue(low);
	m_ui->joyCenterSpin->setValue(center);
	m_ui->joyHighEndSpin->setValue(high);
	m_ui->deadZoneSpin->setValue(deadzone);
}

void PageCTLWgt::FromClassToUi()
{
	m_ui->joyLowEndSpin->setValue(mStgs->joystick.JoyLowEnd);
	m_ui->joyCenterSpin->setValue(mStgs->joystick.JoyCenter);
	m_ui->joyHighEndSpin->setValue(mStgs->joystick.JoyHighEnd);
	m_ui->expFactorSpin->setValue(mStgs->joystick.ExpFactor);
	m_ui->deadZoneSpin->setValue((float)mStgs->joystick.DeadZone/10);
	m_ui->reverseJoyChk->setChecked(mStgs->joystick.JoyFlags & JOY_REVERSE);

	for(int i=0; i<9; i++){
		timeSpin[i].setValue(mStgs->control.Timeout[i]);
	}
	for(int i=0; i<10; i++){
		speedSpin[i].setBaseValue(mStgs->control.MaxSpeed[i], mStgs->control.uMaxSpeed[i]);
	}

	// Set current index of controlModeCombo to 0, 1 or 2, depending on mStgs->control.control_flags first two bits
	m_ui->controlModeCombo->setCurrentIndex(mStgs->control.Flags & (CONTROL_MODE_JOY | CONTROL_MODE_LR));

	if(mStgs->control.Flags & CONTROL_BTN_LEFT_PUSHED_OPEN)
		m_ui->leftBtnPushed->setCurrentIndex(1);
	else
		m_ui->leftBtnPushed->setCurrentIndex(0);

	if(mStgs->control.Flags & CONTROL_BTN_RIGHT_PUSHED_OPEN)
		m_ui->rightBtnPushed->setCurrentIndex(1);
	else
		m_ui->rightBtnPushed->setCurrentIndex(0);

	m_ui->maxClickTimeSpin->setValue(mStgs->control.MaxClickTime);

	m_ui->deltaPositionBox->setBaseValue(mStgs->control.DeltaPosition, mStgs->control.uDeltaPosition);
}

void PageCTLWgt::FromUiToClass(MotorSettings *lmStgs)
{
	lmStgs->control.Flags = 0;
	lmStgs->joystick.JoyFlags = 0;

	lmStgs->joystick.JoyLowEnd = m_ui->joyLowEndSpin->value();
	lmStgs->joystick.JoyCenter = m_ui->joyCenterSpin->value();
	lmStgs->joystick.JoyHighEnd = m_ui->joyHighEndSpin->value();
	lmStgs->joystick.ExpFactor = m_ui->expFactorSpin->value();
	lmStgs->joystick.DeadZone = m_ui->deadZoneSpin->value()*10;
	setUnsetBit(m_ui->reverseJoyChk->isChecked(), &lmStgs->joystick.JoyFlags, JOY_REVERSE);

	for(int i=0; i<9; i++){
		lmStgs->control.Timeout[i] = timeSpin[i].value();
	}
	for(int i=0; i<10; i++){
		lmStgs->control.MaxSpeed[i] = speedSpin[i].UgetBaseStep();
		lmStgs->control.uMaxSpeed[i] = speedSpin[i].UgetBaseUStep();
	}

	// Set lmStgs->control.control_flags first two bits to 0, 1 or 2, depending on current index of controlModeCombo 
	if(m_ui->controlModeCombo->currentIndex() == 1) {
		lmStgs->control.Flags |= CONTROL_MODE_JOY;
		lmStgs->control.Flags &= ~CONTROL_MODE_LR;
	}
	else if(m_ui->controlModeCombo->currentIndex() == 2) {
		lmStgs->control.Flags |= CONTROL_MODE_LR;
		lmStgs->control.Flags &= ~CONTROL_MODE_JOY;
	}
	else {
		lmStgs->control.Flags &= ~(CONTROL_MODE_JOY | CONTROL_MODE_LR);
	}

	setUnsetBit((m_ui->leftBtnPushed->currentIndex() == 1), &lmStgs->control.Flags, CONTROL_BTN_LEFT_PUSHED_OPEN);
	setUnsetBit((m_ui->rightBtnPushed->currentIndex() == 1), &lmStgs->control.Flags, CONTROL_BTN_RIGHT_PUSHED_OPEN);

	lmStgs->control.MaxClickTime =  m_ui->maxClickTimeSpin->value();

	lmStgs->control.DeltaPosition = m_ui->deltaPositionBox->getBaseStep();
	lmStgs->control.uDeltaPosition = m_ui->deltaPositionBox->getBaseUStep();
}
