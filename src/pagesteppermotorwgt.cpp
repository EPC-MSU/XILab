#include <pagesteppermotorwgt.h>
#include <ui_pagesteppermotorwgt.h>
#include <functions.h>

PageStepperMotorWgt::PageStepperMotorWgt(QWidget *parent, MotorSettings *motorStgs) :
    QWidget(parent),
    m_ui(new Ui::PageStepperMotorWgtClass)
{
    m_ui->setupUi(this);

	mStgs = motorStgs;

	QObject::connect(m_ui->stepmodeBox,	   SIGNAL(currentIndexChanged(int)), this, SLOT(OnStepModeChanged(int)));
	QObject::connect(m_ui->limitSpeedChk,  SIGNAL(toggled(bool)),            this, SLOT(OnSpeedChkUnchecked()));
	QObject::connect(m_ui->useMaxSpeedChk, SIGNAL(toggled(bool)),            this, SLOT(OnSpeedChkUnchecked()));
	QObject::connect(m_ui->encoderRadio, SIGNAL(toggled(bool)), this, SLOT(OnEncoderRadioChecked(bool)));
	QObject::connect(m_ui->encoderMediatedRadio, SIGNAL(toggled(bool)), this, SLOT(OnEncoderMediatedRadioChecked(bool)));
	QObject::connect(m_ui->feedbackNoneRadio, SIGNAL(toggled(bool)), this, SLOT(OnNoneRadioChecked(bool)));
	QObject::connect(m_ui->feedbackEmfRadio,  SIGNAL(toggled(bool)),         this, SLOT(OnEmfRadioChecked(bool)));
	OnSpeedChkUnchecked();

	m_ui->label_3->setStyleSheet("QLabel { color : #B0A000; }");
	m_ui->feedbackNoneRadio->setStyleSheet("QRadioButton { color : #B0A000; }");
	m_ui->feedbackEmfRadio->setStyleSheet("QRadioButton { color : #B0A000; }");
	m_ui->encoderRadio->setStyleSheet("QRadioButton { color : #B0A000; }");
	m_ui->encoderMediatedRadio->setStyleSheet("QRadioButton { color : #B0A000; }");
	m_ui->reversChk->setStyleSheet("QCheckBox { color : #B0A000; }");

	m_ui->playBox->setUnitType(UserUnitSettings::TYPE_COORD);
	m_ui->speedBox->setUnitType(UserUnitSettings::TYPE_SPEED);
	m_ui->maxSpeedBox->setUnitType(UserUnitSettings::TYPE_SPEED);
	m_ui->antiplaySpeedBox->setUnitType(UserUnitSettings::TYPE_SPEED);
	m_ui->accelBox->setUnitType(UserUnitSettings::TYPE_ACCEL);
	m_ui->decelBox->setUnitType(UserUnitSettings::TYPE_ACCEL);
#ifndef SERVICEMODE
	//m_ui->resistanceChk->setDisabled(true);
	//m_ui->inductanceChk->setDisabled(true);
#endif
}

PageStepperMotorWgt::~PageStepperMotorWgt()
{
    delete m_ui;
}

void PageStepperMotorWgt::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PageStepperMotorWgt::emitOnRadio()
{
	if (m_ui->encoderMediatedRadio->isChecked()) {
		emit SgnSwitchCTP(FEEDBACK_ENCODER_MEDIATED);
	}
	if (m_ui->encoderRadio->isChecked()) {
		emit SgnSwitchCTP(FEEDBACK_ENCODER);
	}
	if (m_ui->feedbackNoneRadio->isChecked()) {
		emit SgnSwitchCTP(FEEDBACK_NONE);
	}
	if (m_ui->feedbackEmfRadio->isChecked()) {
		emit SgnSwitchCTP(FEEDBACK_EMF);
	}
}

unsigned int PageStepperMotorWgt::OnRadio()
{
	if (m_ui->encoderMediatedRadio->isChecked()) {
		return FEEDBACK_ENCODER_MEDIATED;
	}
	if (m_ui->encoderRadio->isChecked()) {
		return FEEDBACK_ENCODER;
	}
	if (m_ui->feedbackNoneRadio->isChecked()) {
		return FEEDBACK_NONE;
	}
	if (m_ui->feedbackEmfRadio->isChecked()) {
		return FEEDBACK_EMF;
	}
	return 0;
}

void PageStepperMotorWgt::stepmodeBoxDisable()
{
	m_ui->stepmodeBox->setEnabled(false);
	m_ui->stepmodeBox->setToolTip("The microstep mode cannot be changed when working in user units.");
}

void PageStepperMotorWgt::stepmodeBoxEnable()
{
	m_ui->stepmodeBox->setEnabled(true);
	m_ui->stepmodeBox->setToolTip("");
}

void PageStepperMotorWgt::showEvent(QShowEvent *se)
{
	Q_UNUSED(se)
	emitOnRadio();
}

void PageStepperMotorWgt::conditionalBlockSignals(bool is_emit, bool should_block)
{
	if (!is_emit) {
		m_ui->encoderMediatedRadio->blockSignals(should_block);
		m_ui->encoderRadio->blockSignals(should_block);
		m_ui->feedbackNoneRadio->blockSignals(should_block);
		m_ui->feedbackEmfRadio->blockSignals(should_block);
	}
}

void PageStepperMotorWgt::FromClassToUi(bool _emit)
{
	// Must happen first, because it might trigger encoder feedback change and thus change all stepspinboxes
	conditionalBlockSignals(_emit, true);
	m_ui->encoderMediatedRadio->setChecked(mStgs->feedback.FeedbackType == FEEDBACK_ENCODER_MEDIATED);
	m_ui->encoderRadio->setChecked(mStgs->feedback.FeedbackType == FEEDBACK_ENCODER);
	m_ui->feedbackNoneRadio->setChecked(mStgs->feedback.FeedbackType == FEEDBACK_NONE);
	m_ui->feedbackEmfRadio->setChecked(mStgs->feedback.FeedbackType == FEEDBACK_EMF);
	// Enable access to the page EMFestimator when you select EMF.
	if (mStgs->feedback.FeedbackType == FEEDBACK_EMF)
	{
		m_ui->backEMFestimator->setEnabled(TRUE);
		//settingsDlg->pageWgtsLst[PagePidNum]->setFlags(TRUE);//(motorStgs->entype.EngineType != ENGINE_TYPE_STEP) ? (Qt::ItemIsSelectable | Qt::ItemIsEnabled) : (Qt::NoItemFlags));
	}
	if (mStgs->feedback.FeedbackType == FEEDBACK_ENCODER)
	{
		m_ui->backEMFestimator->setEnabled(TRUE);
#ifndef SERVICEMODE
		m_ui->backEMFestimator->setDisabled(TRUE);
#endif		
	}
	if (mStgs->feedback.FeedbackType == (FEEDBACK_NONE | FEEDBACK_ENCODER_MEDIATED))
	{
		m_ui->backEMFestimator->setDisabled(TRUE);
	}

	conditionalBlockSignals(_emit, false);

	if (_emit) {
		emitOnRadio();
	}

    m_ui->limitSpeedChk->setChecked(mStgs->engine.EngineFlags & ENGINE_LIMIT_RPM);
    m_ui->playChk->setChecked(mStgs->engine.EngineFlags & ENGINE_ANTIPLAY);
    m_ui->reversChk->setChecked(mStgs->engine.EngineFlags & ENGINE_REVERSE);
    m_ui->useMaxSpeedChk->setChecked(mStgs->engine.EngineFlags & ENGINE_MAX_SPEED);
    m_ui->accelChk->setChecked(mStgs->engine.EngineFlags & ENGINE_ACCEL_ON);
    m_ui->currentAsRMSchk->setChecked(mStgs->engine.EngineFlags & ENGINE_CURRENT_AS_RMS);
 
	m_ui->ratedCurrBox->setValue(mStgs->engine.NomCurrent);
	m_ui->maxSpeedBox->setBaseValue(mStgs->engine.NomSpeed, mStgs->engine.uNomSpeed);
	m_ui->playBox->setBaseValue(mStgs->engine.Antiplay);
	m_ui->speedBox->setBaseValue(mStgs->move.Speed, mStgs->move.uSpeed);
	m_ui->antiplaySpeedBox->setBaseValue(mStgs->move.AntiplaySpeed, mStgs->move.uAntiplaySpeed);
	m_ui->accelBox->setBaseValue(mStgs->move.Accel, 0);
	m_ui->decelBox->setBaseValue(mStgs->move.Decel, 0);
	m_ui->stepsPerTurnBox->setValue(mStgs->engine.StepsPerRev);

	{
		int j;
		switch (mStgs->engine.MicrostepMode) {
			case MICROSTEP_MODE_FULL: j = 0; break;
			case MICROSTEP_MODE_FRAC_2: j = 1; break;
			case MICROSTEP_MODE_FRAC_4: j = 2; break;
			case MICROSTEP_MODE_FRAC_8: j = 3; break;
			case MICROSTEP_MODE_FRAC_16: j = 4; break;
			case MICROSTEP_MODE_FRAC_32: j = 5; break;
			case MICROSTEP_MODE_FRAC_64: j = 6; break;
			case MICROSTEP_MODE_FRAC_128: j = 7; break;
			case MICROSTEP_MODE_FRAC_256: j = 8; break;
			default: j = 0;
		}
		m_ui->stepmodeBox->setCurrentIndex(j);
	}

	uint32_t value = mStgs->feedback.IPS;
	if (value == 0) {
		value = mStgs->feedback.CountsPerTurn;
	}
	m_ui->cptBox->setValue(value);

	m_ui->encoderReverseChk->setChecked(mStgs->feedback.FeedbackFlags & FEEDBACK_ENC_REVERSE);

	if ((mStgs->feedback.FeedbackFlags & FEEDBACK_ENC_TYPE_BITS) == FEEDBACK_ENC_TYPE_AUTO)
		m_ui->feedbackTypeBox->setCurrentIndex(0);
	else if ((mStgs->feedback.FeedbackFlags & FEEDBACK_ENC_TYPE_BITS) == FEEDBACK_ENC_TYPE_SINGLE_ENDED)
		m_ui->feedbackTypeBox->setCurrentIndex(1);
	else if ((mStgs->feedback.FeedbackFlags & FEEDBACK_ENC_TYPE_BITS) == FEEDBACK_ENC_TYPE_DIFFERENTIAL)
		m_ui->feedbackTypeBox->setCurrentIndex(2);
	else
		m_ui->feedbackTypeBox->setCurrentIndex(-1);

	//Device configuration->Stepper motor->BackEMF estimator
	m_ui->cptBox_inds->setValue(mStgs->emf.L);
	m_ui->cptBox_res->setValue(mStgs->emf.R);
	m_ui->cptBox_Km->setValue(mStgs->emf.Km);
	if ((mStgs->emf.BackEMFFlags & BACK_EMF_INDUCTANCE_AUTO) == BACK_EMF_INDUCTANCE_AUTO)
		m_ui->inductanceChk->setChecked(TRUE);
	else m_ui->inductanceChk->setChecked(FALSE);
	if ((mStgs->emf.BackEMFFlags & BACK_EMF_RESISTANCE_AUTO) == BACK_EMF_RESISTANCE_AUTO)
		m_ui->resistanceChk->setChecked(TRUE);
	else m_ui->resistanceChk->setChecked(FALSE);
	if ((mStgs->emf.BackEMFFlags & BACK_EMF_KM_AUTO) == BACK_EMF_KM_AUTO)
		m_ui->KmChk->setChecked(TRUE);
	else m_ui->KmChk->setChecked(FALSE);
	
}

void PageStepperMotorWgt::FromUiToClass(MotorSettings *lmStgs)
{
	lmStgs->engine.EngineFlags = 0;
	lmStgs->feedback.FeedbackFlags = 0;
	lmStgs->move.MoveFlags = 0;

	setUnsetBit(m_ui->limitSpeedChk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_LIMIT_RPM);
	setUnsetBit(m_ui->playChk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_ANTIPLAY);
	setUnsetBit(m_ui->reversChk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_REVERSE);
	setUnsetBit(m_ui->useMaxSpeedChk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_MAX_SPEED);
	setUnsetBit(m_ui->accelChk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_ACCEL_ON);
	setUnsetBit(m_ui->currentAsRMSchk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_CURRENT_AS_RMS);

	lmStgs->engine.NomCurrent = m_ui->ratedCurrBox->value();
	lmStgs->engine.NomSpeed = m_ui->maxSpeedBox->UgetBaseStep();
	lmStgs->engine.uNomSpeed = m_ui->maxSpeedBox->UgetBaseUStep();
	lmStgs->engine.Antiplay = m_ui->playBox->basevalueInt();
	lmStgs->engine.StepsPerRev = m_ui->stepsPerTurnBox->value();
	switch (m_ui->stepmodeBox->currentIndex()){
	case 0:	lmStgs->engine.MicrostepMode = MICROSTEP_MODE_FULL; break;
	case 1: lmStgs->engine.MicrostepMode = MICROSTEP_MODE_FRAC_2; break;
	case 2: lmStgs->engine.MicrostepMode = MICROSTEP_MODE_FRAC_4; break;
	case 3: lmStgs->engine.MicrostepMode = MICROSTEP_MODE_FRAC_8; break;
	case 4: lmStgs->engine.MicrostepMode = MICROSTEP_MODE_FRAC_16; break;
	case 5: lmStgs->engine.MicrostepMode = MICROSTEP_MODE_FRAC_32; break;
	case 6: lmStgs->engine.MicrostepMode = MICROSTEP_MODE_FRAC_64; break;
	case 7: lmStgs->engine.MicrostepMode = MICROSTEP_MODE_FRAC_128; break;
	case 8: lmStgs->engine.MicrostepMode = MICROSTEP_MODE_FRAC_256; break;
	}

	lmStgs->move.Speed = m_ui->speedBox->UgetBaseStep();
	lmStgs->move.uSpeed = m_ui->speedBox->UgetBaseUStep();
	lmStgs->move.AntiplaySpeed = m_ui->antiplaySpeedBox->UgetBaseStep();
	lmStgs->move.uAntiplaySpeed = m_ui->antiplaySpeedBox->UgetBaseUStep();
	lmStgs->move.Accel = m_ui->accelBox->getBaseStep();
	lmStgs->move.Decel = m_ui->decelBox->getBaseStep();

	if (m_ui->encoderMediatedRadio->isChecked())
	{
		lmStgs->feedback.FeedbackType = FEEDBACK_ENCODER_MEDIATED;
		m_ui->backEMFestimator->setDisabled(TRUE);
	}
	if (m_ui->encoderRadio->isChecked())
	{
		lmStgs->feedback.FeedbackType = FEEDBACK_ENCODER;
		m_ui->backEMFestimator->setEnabled(TRUE);
#ifndef SERVICEMODE
		m_ui->backEMFestimator->setDisabled(TRUE);
#endif	
	}
	if (m_ui->feedbackNoneRadio->isChecked())
	{
		lmStgs->feedback.FeedbackType = FEEDBACK_NONE;
		m_ui->backEMFestimator->setDisabled(TRUE);
	}
	if (m_ui->feedbackEmfRadio->isChecked())
	{
		lmStgs->feedback.FeedbackType = FEEDBACK_EMF;
		m_ui->backEMFestimator->setEnabled(TRUE);
	}

	uint32_t value = m_ui->cptBox->value();
	lmStgs->feedback.CountsPerTurn = value;
	copyOrZero(value, &lmStgs->feedback.IPS);
	setUnsetBit(m_ui->encoderReverseChk->isChecked(), &lmStgs->feedback.FeedbackFlags, FEEDBACK_ENC_REVERSE);

	setUnsetBit((m_ui->feedbackTypeBox->currentIndex() == 0), &lmStgs->feedback.FeedbackFlags, FEEDBACK_ENC_TYPE_AUTO);
	setUnsetBit((m_ui->feedbackTypeBox->currentIndex() == 1), &lmStgs->feedback.FeedbackFlags, FEEDBACK_ENC_TYPE_SINGLE_ENDED);
	setUnsetBit((m_ui->feedbackTypeBox->currentIndex() == 2), &lmStgs->feedback.FeedbackFlags, FEEDBACK_ENC_TYPE_DIFFERENTIAL);

	//Device configuration->Stepper motor->BackEMF estimator
	lmStgs->emf.L = m_ui->cptBox_inds->value();
	lmStgs->emf.R = m_ui->cptBox_res->value();
	lmStgs->emf.Km = m_ui->cptBox_Km->value();
	if (m_ui->inductanceChk->checkState())
		lmStgs->emf.BackEMFFlags |= BACK_EMF_INDUCTANCE_AUTO;
	else
		lmStgs->emf.BackEMFFlags &= ~BACK_EMF_INDUCTANCE_AUTO;
	if (m_ui->resistanceChk->checkState())
		lmStgs->emf.BackEMFFlags |= BACK_EMF_RESISTANCE_AUTO;
	else
		lmStgs->emf.BackEMFFlags &= ~BACK_EMF_RESISTANCE_AUTO;
	if (m_ui->KmChk->checkState())
		lmStgs->emf.BackEMFFlags |= BACK_EMF_KM_AUTO;
	else
		lmStgs->emf.BackEMFFlags &= ~BACK_EMF_KM_AUTO;
}

void PageStepperMotorWgt::OnStepModeChanged(int index)
{
	emit SgnChangeFrac(1 << index);
}

void PageStepperMotorWgt::OnSpeedChkUnchecked()
{
	if ((m_ui->useMaxSpeedChk->isChecked() == false) && (m_ui->limitSpeedChk->isChecked() == false))
		m_ui->maxSpeedBox->setDisabled(true);
	else 
		m_ui->maxSpeedBox->setEnabled(true);
}

void PageStepperMotorWgt::OnEncoderRadioChecked(bool checked)
{
	if (checked) {
		emit SgnSwitchCTP(FEEDBACK_ENCODER);
		emit SgnStepperMotor(FEEDBACK_ENCODER);
	}
}

void PageStepperMotorWgt::OnEncoderMediatedRadioChecked(bool checked)
{
	if (checked) {
		emit SgnSwitchCTP(FEEDBACK_ENCODER_MEDIATED);
		emit SgnStepperMotor(FEEDBACK_ENCODER_MEDIATED);
	}
}

void PageStepperMotorWgt::OnNoneRadioChecked(bool checked)
{
	if (checked) {
		emit SgnSwitchCTP(FEEDBACK_NONE);
		emit SgnStepperMotor(FEEDBACK_NONE);
	}
}

void PageStepperMotorWgt::OnEmfRadioChecked(bool checked)
{
	if (checked) {
		emit SgnSwitchCTP(FEEDBACK_EMF);
		emit SgnStepperMotor(FEEDBACK_EMF);
	}
}

void PageStepperMotorWgt::disableDifferentialFeedback (bool flag)
{
	disableComboboxElement(m_ui->feedbackTypeBox, 2, flag);
	disableComboboxElement(m_ui->feedbackTypeBox, 0, flag);  // disable autodetect too
}
