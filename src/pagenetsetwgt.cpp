#include <ui_pagenetsetwgt.h>
#include <QSpinBox>
#include <QMessageBox>
#include <pagenetsetwgt.h>
#include <functions.h>
#include <commonvars.h>
#include <infobox.h>
#include <string.h>


PageNetSetWgt::PageNetSetWgt(QWidget *parent, UpdateThread *_updateThread, NetworkSettings *_net_sets, DeviceInterface *_devinterface) :
    QWidget(parent),
    m_ui(new Ui::PageNetSetClass)
{
	devinterface = _devinterface;
	pnetsets = _net_sets;
	updateThread = _updateThread;

    m_ui->setupUi(this);

   QObject::connect(m_ui->wrPswBtn, SIGNAL(clicked()), this, SLOT(OnWrPswBtnPressed()));
   QObject::connect(m_ui->wrBtn, SIGNAL(clicked()), this, SLOT(OnWrBtnPressed()));
}

PageNetSetWgt::~PageNetSetWgt()
{
    delete m_ui;
}


void PageNetSetWgt::FromClassToUi()
{
    m_ui->passwLe->setText(pnetsets->passw);

    m_ui->hwBox_ip0->setValue(pnetsets->net_set.IPv4Address[0]);
    m_ui->hwBox_ip1->setValue(pnetsets->net_set.IPv4Address[1]);
    m_ui->hwBox_ip2->setValue(pnetsets->net_set.IPv4Address[2]);
    m_ui->hwBox_ip3->setValue(pnetsets->net_set.IPv4Address[3]);

    m_ui->hwBox_sbm0->setValue(pnetsets->net_set.SubnetMask[0]);
    m_ui->hwBox_sbm1->setValue(pnetsets->net_set.SubnetMask[1]);
    m_ui->hwBox_sbm2->setValue(pnetsets->net_set.SubnetMask[2]);
    m_ui->hwBox_sbm3->setValue(pnetsets->net_set.SubnetMask[3]);

    m_ui->hwBox_dgw0->setValue(pnetsets->net_set.DefaultGateway[0]);
    m_ui->hwBox_dgw1->setValue(pnetsets->net_set.DefaultGateway[1]);
    m_ui->hwBox_dgw2->setValue(pnetsets->net_set.DefaultGateway[2]);
    m_ui->hwBox_dgw3->setValue(pnetsets->net_set.DefaultGateway[3]);
    m_ui->checkBox->setChecked(pnetsets->net_set.DHCPEnabled != 0);
}

void PageNetSetWgt::FromUiToClass()
{
    pnetsets->passw = m_ui->passwLe->text();

    pnetsets->net_set.IPv4Address[0] = (uint8_t)(m_ui->hwBox_ip0->text().toUInt());
    pnetsets->net_set.IPv4Address[1] = (uint8_t)(m_ui->hwBox_ip1->text().toUInt());
    pnetsets->net_set.IPv4Address[2] = (uint8_t)(m_ui->hwBox_ip2->text().toUInt());
    pnetsets->net_set.IPv4Address[3] = (uint8_t)(m_ui->hwBox_ip3->text().toUInt());


    pnetsets->net_set.SubnetMask[0] = (uint8_t)(m_ui->hwBox_sbm0->text().toUInt());
    pnetsets->net_set.SubnetMask[1] = (uint8_t)(m_ui->hwBox_sbm1->text().toUInt());
    pnetsets->net_set.SubnetMask[2] = (uint8_t)(m_ui->hwBox_sbm2->text().toUInt());
    pnetsets->net_set.SubnetMask[3] = (uint8_t)(m_ui->hwBox_sbm3->text().toUInt());


    pnetsets->net_set.DefaultGateway[0] = (uint8_t)(m_ui->hwBox_dgw0->text().toUInt());
    pnetsets->net_set.DefaultGateway[1] = (uint8_t)m_ui->hwBox_dgw1->text().toUInt();
    pnetsets->net_set.DefaultGateway[2] = (uint8_t)m_ui->hwBox_dgw2->text().toUInt();
    pnetsets->net_set.DefaultGateway[3] = (uint8_t)m_ui->hwBox_dgw3->text().toUInt();
    pnetsets->net_set.DHCPEnabled = m_ui->checkBox->isChecked() ? 1 : 0;
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

static void from_qstr_touint20(const QString& s, unsigned int inta[20])
{
    QString d = s;
    if (d.size() > 19)
        d.resize(19);
    int i = 0;
    for (auto ch : d)
    {
        inta[i++] = (unsigned int)ch.unicode();
    }
    inta[i] = 0;
}

void PageNetSetWgt::OnWrPswBtnPressed()
{
    FromUiToClass();
    bool saved_state = devinterface->cs->updatesEnabled();
    devinterface->cs->setUpdatesEnabled(false);

    InfoBox infoBox;
    infoBox.setButtonsVisible(false);
    infoBox.setMovieVisible(true);
    infoBox.setIcon(QMessageBox::Information);
    infoBox.setText("Please wait while web interface password of network settings is being updated");
    infoBox.show();
    QApplication::processEvents();
    
    password_settings_t pst;
    memset(&pst, 0, sizeof(password_settings_t));
    from_qstr_touint20(pnetsets->passw, pst.UserPassword);

    result_t result = devinterface->set_password_settings(&pst);

    devinterface->cs->setUpdatesEnabled(saved_state);

    if (result == result_ok){
        
        QString pwd = pnetsets->passw;
        pnetsets->LoadPasswordSettings();
        if(pwd == pnetsets->passw){
        infoBox.setIcon(QMessageBox::Information);
        infoBox.setButtons(QDialogButtonBox::Ok);
        infoBox.setMovieVisible(false);
        infoBox.setText("Web page password of network settings was updated succesfully");
        infoBox.exec();
        }
        else{
        infoBox.setIcon(QMessageBox::Critical);
        infoBox.setMovieVisible(false);
        infoBox.setButtons(QDialogButtonBox::Ok);
        infoBox.setText(QString("Command was executed succesfully, but password hasn't been changed"));
        infoBox.exec();
        }
     }
    else{
        QString error_code;
        switch (result){
        case result_error: error_code = "generic error"; break;
        case result_not_implemented: error_code = "function is not implemented"; break;
        case result_nodevice: error_code = "device is lost"; break;
        default: error_code = "unknown error"; break;
        }
        infoBox.setIcon(QMessageBox::Critical);
        infoBox.setMovieVisible(false);
        infoBox.setButtons(QDialogButtonBox::Ok);
        infoBox.setText("Web page password of network settings updating error.\nReturned value: " + error_code);
        infoBox.exec();
    }
}

void PageNetSetWgt::OnWrBtnPressed()
{
    FromUiToClass();

	bool saved_state = devinterface->cs->updatesEnabled();
	devinterface->cs->setUpdatesEnabled(false);

	InfoBox infoBox;
	infoBox.setButtonsVisible(false);
	infoBox.setMovieVisible(true);
	infoBox.setIcon(QMessageBox::Information);
	infoBox.setText("Please wait while network settings are being updated");
	infoBox.show();
	QApplication::processEvents();

	result_t result = devinterface->set_network_settings(&(pnetsets->net_set));
	
	devinterface->cs->setUpdatesEnabled(saved_state);
	
	if(result == result_ok){
       
        network_settings_t _tnst;
        memcpy((void *)&_tnst, (void *)&pnetsets->net_set, sizeof(network_settings_t));
		pnetsets->LoadRestNetSettings();
        if (memcmp((void *)&_tnst, (void *)&(pnetsets->net_set), sizeof(network_settings_t)) == 0){
			infoBox.setIcon(QMessageBox::Information);
			infoBox.setButtons(QDialogButtonBox::Ok);
			infoBox.setMovieVisible(false);
			infoBox.setText("Network settings were updated succesfully");
			infoBox.exec();
			
		}
		else{
			infoBox.setIcon(QMessageBox::Critical);
			infoBox.setMovieVisible(false);
			infoBox.setButtons(QDialogButtonBox::Ok);
    pnetsets->net_set.DHCPEnabled = m_ui->checkBox->isChecked() ? 1 : 0;
    infoBox.setText(QString("Command was executed succesfully, but network settings havn't been changed.") + 
                    QString(pnetsets->net_set.DHCPEnabled ? "\nProbably, you switched on the DHCP server option, then the other settings cannot be applied." : ""));
			infoBox.exec();
		}
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


