#include <pagepidwgt.h>
#include <ui_pagepidwgt.h>

#include <qmessagebox.h>

PagePidWgt::PagePidWgt(QWidget *parent, MotorSettings *motorStgs, DeviceInterface *_devinterface, uint32_t _serial) :
	QWidget(parent),
	m_ui(new Ui::PagePidWgtClass), calibrator_window(_devinterface, _serial)
{
    m_ui->setupUi(this);

	m_ui->dVoltBox->installEventFilter(this);
	m_ui->iVoltBox->installEventFilter(this);
	m_ui->pVoltBox->installEventFilter(this);

    mStgs = motorStgs;
	this->devface = _devinterface;

	QObject::connect(this->m_ui->pidTuningBtn, SIGNAL(clicked()), this, SLOT(onPIDTune()));
	QObject::connect(&(this->calibrator_window), 
					 SIGNAL(changePIDSettings(float, float, float, int, int, int)), 
					 this, 
					 SLOT(setPIDSettings(float, float, float, int, int, int)));

}

PagePidWgt::~PagePidWgt()
{
    delete m_ui;
}

void PagePidWgt::validate(void)
{
	
	if (m_ui->dVoltBox->value() > 65535)
		m_ui->dVoltBox->setValue(65535);
	if (m_ui->iVoltBox->value() > 65535)
		m_ui->iVoltBox->setValue(65535);
	if (m_ui->pVoltBox->value() > 65535)
		m_ui->pVoltBox->setValue(65535);
}

bool  PagePidWgt::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() == QEvent::FocusOut)
	{
		if (object == m_ui->dVoltBox || object == m_ui->iVoltBox || object == m_ui->pVoltBox)
		{
			PagePidWgt::validate();
		}
	}
	return false;
}

void PagePidWgt::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;	
    default:
        break;
    }
}

void PagePidWgt::FromClassToUi()
{
    m_ui->pVoltBox->setValue(mStgs->pid.KpU);
    m_ui->iVoltBox->setValue(mStgs->pid.KiU);
    m_ui->dVoltBox->setValue(mStgs->pid.KdU);

    m_ui->fpVoltBox->setValue(mStgs->pid.Kpf * 1000.0F);
    m_ui->fiVoltBox->setValue(mStgs->pid.Kif * 1000.0F);
    m_ui->fdVoltBox->setValue(mStgs->pid.Kdf * 1000.0F);
}

void PagePidWgt::FromUiToClass(MotorSettings *lmStgs)
{
    lmStgs->pid.KpU = m_ui->pVoltBox->value();
    lmStgs->pid.KiU = m_ui->iVoltBox->value();
    lmStgs->pid.KdU = m_ui->dVoltBox->value();

    lmStgs->pid.Kpf = m_ui->fpVoltBox->value() / 1000.0F;
    lmStgs->pid.Kif = m_ui->fiVoltBox->value() / 1000.0F;
    lmStgs->pid.Kdf = m_ui->fdVoltBox->value() / 1000.0F;
}

void PagePidWgt::SetMotorType(unsigned int entype)
{
	m_ui->fpVoltBox->setEnabled((entype == ENGINE_TYPE_BRUSHLESS) || (entype == ENGINE_TYPE_STEP));
	m_ui->fiVoltBox->setEnabled(entype == ENGINE_TYPE_BRUSHLESS || (entype == ENGINE_TYPE_STEP));
	m_ui->fdVoltBox->setEnabled(entype == ENGINE_TYPE_BRUSHLESS || (entype == ENGINE_TYPE_STEP));
	m_ui->pVoltBox->setEnabled(entype == ENGINE_TYPE_DC);
	m_ui->iVoltBox->setEnabled(entype == ENGINE_TYPE_DC);
	m_ui->dVoltBox->setEnabled(entype == ENGINE_TYPE_DC);

#ifdef SERVICEMODE  // refs to #23173
	//m_ui->fpVoltBox->setEnabled(true);
	//m_ui->fiVoltBox->setEnabled(true);
	//m_ui->fdVoltBox->setEnabled(true);
#endif

	this->m_ui->pidTuningBtn->setEnabled((entype == ENGINE_TYPE_BRUSHLESS) || (entype == ENGINE_TYPE_DC) || (entype == ENGINE_TYPE_STEP));
}

void PagePidWgt::onPIDTune()
{
	libximc::entype_settings_t settings;
	libximc::result_t result = this->devface->get_entype_settings(&settings);

	if (result != result_ok)
	{
		// TODO: this
		return;
	}

	if (settings.EngineType != ENGINE_TYPE_BRUSHLESS && settings.EngineType != ENGINE_TYPE_DC && settings.EngineType != ENGINE_TYPE_STEP)
	{
		QMessageBox question;
		question.setText("Wrong controller settings");
		question.setInformativeText("Only BLDC and DC support an oscilloscope. The correct settings are set, but not applied to the controller. Apply current settings?");
		question.setStandardButtons(QMessageBox::Apply | QMessageBox::Cancel);

		int ret = question.exec();

		if (ret == QMessageBox::Apply)
		{
			emit this->Apply();
		}
		else
		{
			return;
		}
	}

	this->calibrator_window.show();
	emit this->PIDTuneShowed();
}

void PagePidWgt::setPIDSettings(float Kpf, float Kif, float Kdf, int Kp, int Ki, int Kd)
{
	m_ui->pVoltBox->setValue(Kp);
	m_ui->iVoltBox->setValue(Ki);
	m_ui->dVoltBox->setValue(Kd);

	m_ui->fpVoltBox->setValue(Kpf * 1000.0F);
	m_ui->fiVoltBox->setValue(Kif * 1000.0F);
	m_ui->fdVoltBox->setValue(Kdf * 1000.0F);
}