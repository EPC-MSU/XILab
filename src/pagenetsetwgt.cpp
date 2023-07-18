#include <ui_pagenetsetwgt.h>
#include <QSpinBox>
#include <QMessageBox>
#include <pagenetsetwgt.h>
#include <functions.h>
#include <commonvars.h>
#include <infobox.h>


PageNetSetWgt::PageNetSetWgt(QWidget *parent, UpdateThread *_updateThread, network_settings_t *_net_sets, DeviceInterface *_devinterface) :
    QWidget(parent),
    m_ui(new Ui::PageNetSetClass)
{
	devinterface = _devinterface;
	network_sets = _net_sets;
	updateThread = _updateThread;

    m_ui->setupUi(this);


	m_ui->hwBox_dgw0->setValue(network_sets->DefaultGateway[0]);
    m_ui->hwBox_dgw1->setValue(network_sets->DefaultGateway[1]);
    m_ui->hwBox_dgw2->setValue(network_sets->DefaultGateway[2]);
    m_ui->hwBox_dgw3->setValue(network_sets->DefaultGateway[3]);
    m_ui->checkBox->setChecked(network_sets->DHCPEnabled != 0);

    QObject::connect(m_ui->writeBtn, SIGNAL(clicked()), this, SLOT(OnWriteBtnPressed()));
}

PageNetSetWgt::~PageNetSetWgt()
{
    delete m_ui;
}

void PageNetSetWgt::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PageNetSetWgt::OnWriteBtnPressed()
{
    network_settings_t _nst;
    
    _nst.DefaultGateway[0] = (uint8_t)(m_ui->hwBox_dgw0->text().toUInt());
    _nst.DefaultGateway[1] = (uint8_t)m_ui->hwBox_dgw1->text().toUInt();
    _nst.DefaultGateway[2] = (uint8_t)m_ui->hwBox_dgw2->text().toUInt();
    _nst.DefaultGateway[3] = (uint8_t)m_ui->hwBox_dgw3->text().toUInt();

    _nst.DHCPEnabled = m_ui->checkBox->isChecked() ? 1 : 0;

	bool saved_state = devinterface->cs->updatesEnabled();
	devinterface->cs->setUpdatesEnabled(false);

	InfoBox infoBox;
	infoBox.setButtonsVisible(false);
	infoBox.setMovieVisible(true);
	infoBox.setIcon(QMessageBox::Information);
	infoBox.setText("Please wait while serial number is updating");
	infoBox.show();
	QApplication::processEvents();

	//devinterface->close_device();
	//	sleep_act(2*BOOTLOADER_DELAY);
	//	devinterface->open_device(updateThread->device_name);
	result_t result = devinterface->set_network_settings(&_nst);
	
	devinterface->cs->setUpdatesEnabled(saved_state);
	
	if(result == result_ok){
        /*
		controllerStgs->LoadControllerSettings();
		if(controllerStgs->serialnumber == serial.SN){
			infoBox.setIcon(QMessageBox::Information);
			infoBox.setButtons(QDialogButtonBox::Ok);
			infoBox.setMovieVisible(false);
			infoBox.setText("Serial number was updated succesfully");
			infoBox.exec();
			emit serialUpdatedSgn();
            
		}
		else{
			infoBox.setIcon(QMessageBox::Critical);
			infoBox.setMovieVisible(false);
			infoBox.setButtons(QDialogButtonBox::Ok);
			infoBox.setText(QString("Command was executed succesfully, but SN hasn't changed.\nTarget serial: %1, current serial: %2").arg(serial.SN).arg(controllerStgs->serialnumber));
			infoBox.exec();
		}
	*/
    }
	else{
		QString error_code;
		switch(result){
			case result_error: error_code = "generic error"; break;
			case result_not_implemented: error_code = "function is not implemented"; break;
			case result_nodevice: error_code = "device is lost"; break;
			default: error_code = "unknown error"; break;
		}
		infoBox.setIcon(QMessageBox::Critical);
		infoBox.setMovieVisible(false);
		infoBox.setButtons(QDialogButtonBox::Ok);
		infoBox.setText("Network settings updating error.\nReturned value: " + error_code);
		infoBox.exec();
	}
}


