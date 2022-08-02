#include <pagedcmotorwgt.h>
#include <ui_pagedcmotorwgt.h>
#include <functions.h>

PageDCMotorWgt::PageDCMotorWgt(QWidget *parent, MotorSettings *motorStgs) :
    QWidget(parent),
    m_ui(new Ui::PageDCMotorWgtClass)
{
    m_ui->setupUi(this);
    mStgs = motorStgs;

	QObject::connect(m_ui->limitSpeedChk,	SIGNAL(toggled(bool)), this, SLOT(OnSpeedChkUnchecked()));
	QObject::connect(m_ui->useMaxSpeedChk,	SIGNAL(toggled(bool)), this, SLOT(OnSpeedChkUnchecked()));
	QObject::connect(m_ui->encoderRadio,    SIGNAL(toggled(bool)), this, SLOT(OnEncoderRadioChecked(bool)));
	OnSpeedChkUnchecked();

	m_ui->rpmBox->setUnitType(UserUnitSettings::TYPE_SPEED);
	m_ui->playBox->setUnitType(UserUnitSettings::TYPE_COORD);
	m_ui->antiplaySpeedBox->setUnitType(UserUnitSettings::TYPE_SPEED);
	m_ui->speedBox->setUnitType(UserUnitSettings::TYPE_SPEED);
	m_ui->accelBox->setUnitType(UserUnitSettings::TYPE_ACCEL);
	m_ui->decelBox->setUnitType(UserUnitSettings::TYPE_ACCEL);
	m_ui->hiddenRadio->setHidden(true);
}

PageDCMotorWgt::~PageDCMotorWgt()
{
    delete m_ui;
}

void PageDCMotorWgt::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PageDCMotorWgt::emitOnRadio()
{
	if (m_ui->encoderRadio->isChecked()) {
		emit SgnSwitchCTP(FEEDBACK_ENCODER);
	}
}

void PageDCMotorWgt::showEvent(QShowEvent *se)
{
	Q_UNUSED(se)
	emitOnRadio();
}

void PageDCMotorWgt::FromClassToUi(bool _emit)
{
	// Must happen first, because it might trigger encoder feedback change and thus change all stepspinboxes
	if (!_emit) { m_ui->encoderRadio->blockSignals(true); }
	m_ui->encoderRadio->setChecked(true); // unconditional, because this is a single radiobutton here
	if (!_emit) { m_ui->encoderRadio->blockSignals(false); }

	if (_emit) {
		emitOnRadio();
	}

    m_ui->voltageChk->setChecked(mStgs->engine.EngineFlags & ENGINE_LIMIT_VOLT);
    m_ui->currentChk->setChecked(mStgs->engine.EngineFlags & ENGINE_LIMIT_CURR);
    m_ui->limitSpeedChk->setChecked(mStgs->engine.EngineFlags & ENGINE_LIMIT_RPM);
    m_ui->playChk->setChecked(mStgs->engine.EngineFlags & ENGINE_ANTIPLAY);
    m_ui->reversChk->setChecked(mStgs->engine.EngineFlags & ENGINE_REVERSE);
    m_ui->useMaxSpeedChk->setChecked(mStgs->engine.EngineFlags & ENGINE_MAX_SPEED);
    m_ui->accelChk->setChecked(mStgs->engine.EngineFlags & ENGINE_ACCEL_ON);

    m_ui->voltageBox->setValue(mStgs->engine.NomVoltage * 10.);
    m_ui->currentBox->setValue(mStgs->engine.NomCurrent);
    m_ui->rpmBox->setBaseValue(mStgs->engine.NomSpeed);
    m_ui->playBox->setBaseValue(mStgs->engine.Antiplay);

	m_ui->speedBox->setBaseValue(mStgs->move.Speed);
	m_ui->antiplaySpeedBox->setBaseValue(mStgs->move.AntiplaySpeed);
	m_ui->accelBox->setBaseValue(mStgs->move.Accel);
	m_ui->decelBox->setBaseValue(mStgs->move.Decel);

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
}

void PageDCMotorWgt::FromUiToClass(MotorSettings *lmStgs)
{
	lmStgs->engine.EngineFlags = 0;
	lmStgs->feedback.FeedbackFlags = 0;
	lmStgs->move.MoveFlags = 0;

	setUnsetBit(m_ui->voltageChk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_LIMIT_VOLT);
	setUnsetBit(m_ui->currentChk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_LIMIT_CURR);
	setUnsetBit(m_ui->limitSpeedChk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_LIMIT_RPM);
	setUnsetBit(m_ui->playChk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_ANTIPLAY);
	setUnsetBit(m_ui->reversChk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_REVERSE);
	setUnsetBit(m_ui->useMaxSpeedChk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_MAX_SPEED);
	setUnsetBit(m_ui->accelChk->isChecked(), &lmStgs->engine.EngineFlags, ENGINE_ACCEL_ON);

    lmStgs->engine.NomVoltage = m_ui->voltageBox->value() / 10.;
    lmStgs->engine.NomCurrent = m_ui->currentBox->value();
    lmStgs->engine.NomSpeed = m_ui->rpmBox->basevalueInt();
    lmStgs->engine.Antiplay = m_ui->playBox->basevalueInt();

	lmStgs->move.Speed = m_ui->speedBox->basevalueInt();
	lmStgs->move.AntiplaySpeed = m_ui->antiplaySpeedBox->basevalueInt();
    lmStgs->move.Accel = m_ui->accelBox->basevalueInt();
	lmStgs->move.Decel = m_ui->decelBox->basevalueInt();

    if (m_ui->encoderRadio->isChecked())
		lmStgs->feedback.FeedbackType = FEEDBACK_ENCODER;

	uint32_t value = m_ui->cptBox->value();
	lmStgs->feedback.CountsPerTurn = value;
	copyOrZero(value, &lmStgs->feedback.IPS);
	setUnsetBit(m_ui->encoderReverseChk->isChecked(), &lmStgs->feedback.FeedbackFlags, FEEDBACK_ENC_REVERSE);

	setUnsetBit((m_ui->feedbackTypeBox->currentIndex() == 0), &lmStgs->feedback.FeedbackFlags, FEEDBACK_ENC_TYPE_AUTO);
	setUnsetBit((m_ui->feedbackTypeBox->currentIndex() == 1), &lmStgs->feedback.FeedbackFlags, FEEDBACK_ENC_TYPE_SINGLE_ENDED);
	setUnsetBit((m_ui->feedbackTypeBox->currentIndex() == 2), &lmStgs->feedback.FeedbackFlags, FEEDBACK_ENC_TYPE_DIFFERENTIAL);
}

void PageDCMotorWgt::OnSpeedChkUnchecked()
{
	if ((m_ui->useMaxSpeedChk->isChecked() == false) && (m_ui->limitSpeedChk->isChecked() == false))
		m_ui->rpmBox->setDisabled(true);
	else 
		m_ui->rpmBox->setEnabled(true);
}

void PageDCMotorWgt::OnEncoderRadioChecked(bool checked)
{
	if (checked) {
		emit SgnSwitchCTP(FEEDBACK_ENCODER);
	}
}

void PageDCMotorWgt::disableDifferentialFeedback (bool flag)
{
	disableComboboxElement(m_ui->feedbackTypeBox, 2, flag);
	disableComboboxElement(m_ui->feedbackTypeBox, 0, flag);  // disable autodetect too
}
