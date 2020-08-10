#include <pagelimitswgt.h>
#include <ui_pagelimitswgt.h>
#include <functions.h>

PageLimitsWgt::PageLimitsWgt(QWidget *parent, MotorSettings *motorStgs) :
    QWidget(parent),
    m_ui(new Ui::PageLimitsWgtClass)
{
    m_ui->setupUi(this);

    mStgs = motorStgs;

	m_ui->label_7->setEnabled(m_ui->lowVoltageBox->isChecked());
	m_ui->LowVoltageOffBox->setEnabled(m_ui->lowVoltageBox->isChecked());
}

PageLimitsWgt::~PageLimitsWgt()
{
    delete m_ui;
}

void PageLimitsWgt::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PageLimitsWgt::FromClassToUi()
{
	m_ui->LowVoltageOffBox->setValue(mStgs->secure.LowUpwrOff*10);
    m_ui->MaxCurrentPwrBox->setValue(mStgs->secure.CriticalIpwr);
	m_ui->MaxVoltagePwrBox->setValue(mStgs->secure.CriticalUpwr*10);
	m_ui->MaxCurrentUsbBox->setValue(mStgs->secure.CriticalIusb);
	m_ui->MaxVoltageUsbBox->setValue(mStgs->secure.CriticalUusb*10);
	m_ui->MinVoltageUsbBox->setValue(mStgs->secure.MinimumUusb*10);
    m_ui->MaxTempBox->setValue(mStgs->secure.CriticalT/10.);
	m_ui->overheatBox->setChecked(mStgs->secure.Flags & ALARM_ON_DRIVER_OVERHEATING);
	m_ui->lowVoltageBox->setChecked(mStgs->secure.Flags & LOW_UPWR_PROTECTION);
	m_ui->bridgeBox->setChecked(mStgs->secure.Flags & H_BRIDGE_ALERT);
	m_ui->borderMissetBox->setChecked(mStgs->secure.Flags & ALARM_ON_BORDERS_SWAP_MISSET);
	m_ui->stickyAlarmBox->setChecked(mStgs->secure.Flags & ALARM_FLAGS_STICKING);
	m_ui->usbBreakReconnect->setChecked(mStgs->secure.Flags & USB_BREAK_RECONNECT);
	m_ui->WRMalarm->setChecked(mStgs->secure.Flags & ALARM_WINDING_MISMATCH);
	m_ui->ENGalarm->setChecked(mStgs->secure.Flags & ALARM_ENGINE_RESPONSE);
}

void PageLimitsWgt::FromUiToClass(MotorSettings *lmStgs)
{
	lmStgs->secure.Flags = 0;

	lmStgs->secure.LowUpwrOff = m_ui->LowVoltageOffBox->value()/10;
	lmStgs->secure.CriticalIpwr = m_ui->MaxCurrentPwrBox->value();
	lmStgs->secure.CriticalUpwr = m_ui->MaxVoltagePwrBox->value()/10;
	lmStgs->secure.CriticalIusb = m_ui->MaxCurrentUsbBox->value();
	lmStgs->secure.CriticalUusb = m_ui->MaxVoltageUsbBox->value()/10;
	lmStgs->secure.MinimumUusb = m_ui->MinVoltageUsbBox->value()/10;
    lmStgs->secure.CriticalT = m_ui->MaxTempBox->value()*10;

	setUnsetBit(m_ui->overheatBox->isChecked(), &lmStgs->secure.Flags, ALARM_ON_DRIVER_OVERHEATING);
	setUnsetBit(m_ui->lowVoltageBox->isChecked(), &lmStgs->secure.Flags, LOW_UPWR_PROTECTION);
	setUnsetBit(m_ui->bridgeBox->isChecked(), &lmStgs->secure.Flags, H_BRIDGE_ALERT);
	setUnsetBit(m_ui->borderMissetBox->isChecked(), &lmStgs->secure.Flags, ALARM_ON_BORDERS_SWAP_MISSET);
	setUnsetBit(m_ui->stickyAlarmBox->isChecked(), &lmStgs->secure.Flags, ALARM_FLAGS_STICKING);
	setUnsetBit(m_ui->usbBreakReconnect->isChecked(), &lmStgs->secure.Flags, USB_BREAK_RECONNECT);
	setUnsetBit(m_ui->WRMalarm->isChecked(), &lmStgs->secure.Flags, ALARM_WINDING_MISMATCH);
	setUnsetBit(m_ui->ENGalarm->isChecked(), &lmStgs->secure.Flags, ALARM_ENGINE_RESPONSE);
}

void PageLimitsWgt::disableCriticalUSB(bool value)
{
	m_ui->label_2->setDisabled(value);
	m_ui->label_4->setDisabled(value);
	m_ui->label_5->setDisabled(value);

	m_ui->MaxCurrentUsbBox->setDisabled(value);
	m_ui->MaxVoltageUsbBox->setDisabled(value);
	m_ui->MinVoltageUsbBox->setDisabled(value);
}
