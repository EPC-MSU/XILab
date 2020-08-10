#include <QMessageBox>
#include <pagecalibwgt.h>
#include <ui_pagecalibwgt.h>
#include <functions.h>
#include <main.h>
#include <status.h>

PageCalibWgt::PageCalibWgt(QWidget *parent, QString dev_name, DeviceInterface *_devinterface) :
    QWidget(parent),
    m_ui(new Ui::PageCalibWgtClass)
{
    m_ui->setupUi(this);
	devinterface = _devinterface;

	int i;
	for(i=0; i<dev_name.length(); i++){
		device_name[i] = (char)dev_name[i].toAscii();
	}
	device_name[i] = '\0';

	infoBox = new InfoBox();

	timer = new QTimer(this);
	timer->start(100);

	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(UpdatePageText()));
}

PageCalibWgt::~PageCalibWgt()
{
	delete infoBox;
    delete m_ui;
}

void PageCalibWgt::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PageCalibWgt::UpdatePageText()
{
	state_vars_t state_vars;
	state_vars.connect = devinterface->cs->connect();
	state_vars.analog_data = devinterface->cs->analogData();
	state_vars.chart_data = devinterface->cs->chartData();

	if(state_vars.connect && isVisible()){
		m_ui->adcA1VoltLbl->setNum((int)state_vars.analog_data.A1Voltage_ADC);
		m_ui->cdcA1VoltLbl->setNum(state_vars.analog_data.A1Voltage/100.0);
		m_ui->adcA2VoltLbl->setNum((int)state_vars.analog_data.A2Voltage_ADC);
		m_ui->cdcA2VoltLbl->setNum(state_vars.analog_data.A2Voltage/100.0);
		m_ui->adcB1VoltLbl->setNum((int)state_vars.analog_data.B1Voltage_ADC);
		m_ui->cdcB1VoltLbl->setNum(state_vars.analog_data.B1Voltage/100.0);
		m_ui->adcB2VoltLbl->setNum((int)state_vars.analog_data.B2Voltage_ADC);
		m_ui->cdcB2VoltLbl->setNum(state_vars.analog_data.B2Voltage/100.0);
		m_ui->adcVsupLbl->setNum((int)state_vars.analog_data.SupVoltage_ADC);
		m_ui->cdcVsupLbl->setNum(state_vars.analog_data.SupVoltage/100.0);
		
		m_ui->adcEngCurLbl->setNum((int)state_vars.analog_data.FullCurrent_ADC);
		m_ui->cdcEngCurLbl->setNum(state_vars.analog_data.FullCurrent);
		m_ui->adcACurrLbl->setNum((int)state_vars.analog_data.ACurrent_ADC);
		m_ui->cdcACurrLbl->setNum(state_vars.analog_data.ACurrent);
		m_ui->adcBCurrLbl->setNum((int)state_vars.analog_data.BCurrent_ADC);
		m_ui->cdcBCurrLbl->setNum(state_vars.analog_data.BCurrent);
		
		m_ui->adcPotLbl->setNum((int)state_vars.analog_data.Pot_ADC);
		m_ui->cdcPotLbl->setNum((int)state_vars.analog_data.Pot);
		m_ui->adcJoyLbl->setNum((int)state_vars.analog_data.Joy_ADC);
		m_ui->cdcJoyLbl->setNum((int)state_vars.analog_data.Joy);
		m_ui->adcL5Lbl->setNum((int)state_vars.analog_data.L5_ADC);
		m_ui->cdcL5Lbl->setNum((int)state_vars.analog_data.L5/100.0);
		m_ui->adcH5Lbl->setNum((int)state_vars.analog_data.H5_ADC);
		m_ui->cdcH5Lbl->setNum((int)state_vars.analog_data.H5/100.0);
		m_ui->adcTempKeyLbl->setNum((int)state_vars.analog_data.Temp_ADC);
		m_ui->cdcTempKeyLbl->setNum(state_vars.analog_data.Temp/10.0);

		m_ui->cdcPwmLevelLbl->setNum(state_vars.chart_data.DutyCycle/100.0);
		m_ui->R->setNum(state_vars.analog_data.R);
		m_ui->L->setNum(state_vars.analog_data.L);
	}
}
