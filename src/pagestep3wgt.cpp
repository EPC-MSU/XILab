#include <QSpinBox>
#include <QMessageBox>
#include <pagestep3wgt.h>
#include <functions.h>
#include <commonvars.h>
#include <infobox.h>
#include <ui_pagestep3wgt.h>

PageStep3Wgt::PageStep3Wgt(QWidget *parent, UpdateThread *_updateThread, ControllerSettings *_controllerStgs, DeviceInterface *_devinterface) :
    QWidget(parent),
    m_ui(new Ui::PageStep3WgtClass)
{
	devinterface = _devinterface;
	controllerStgs = _controllerStgs;
	updateThread = _updateThread;

    m_ui->setupUi(this);
	m_ui->snBox->setMaximum(INT_MAX);
	m_ui->snBox->setValue(controllerStgs->serialnumber);

	m_ui->hwBox_major->setValue(controllerStgs->device_info.Major);
	m_ui->hwBox_minor->setValue(controllerStgs->device_info.Minor);
	m_ui->hwBox_release->setValue(controllerStgs->device_info.Release);

	QObject::connect(m_ui->setKeyBtn, SIGNAL(clicked()), this, SLOT(OnSetKeyBtnPressed()));
	QObject::connect(m_ui->writeSnBtn, SIGNAL(clicked()), this, SLOT(OnWriteSNBtnPressed()));
	QObject::connect(m_ui->rebootToBoot, SIGNAL(clicked()), this, SLOT(OnRebootToBoot()));
	key.resize(32);	

	// We check against this version to test if we need to call IRND and return encrypted key
	boot_need_encrypted_key_version = {2, 3, 0};
}

PageStep3Wgt::~PageStep3Wgt()
{
    delete m_ui;
}

void PageStep3Wgt::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PageStep3Wgt::OnWriteSNBtnPressed()
{
	serial_number_t serial;
	serial.SN = m_ui->snBox->value();
	Base16to256(m_ui->keyBox->text(), &key);

	bool saved_state = devinterface->cs->updatesEnabled();
	devinterface->cs->setUpdatesEnabled(false);

	InfoBox infoBox;
	infoBox.setButtonsVisible(false);
	infoBox.setMovieVisible(true);
	infoBox.setIcon(QMessageBox::Information);
	infoBox.setText("Please wait while serial number is updating");
	infoBox.show();
	QApplication::processEvents();

	if ( is_asc_order(boot_need_encrypted_key_version, controllerStgs->bootloader_version) ) {
		init_random_t irnd;
		devinterface->service_command_updf();
		devinterface->close_device();
		sleep_act(2*BOOTLOADER_DELAY);
		devinterface->open_device(updateThread->device_name);
		devinterface->get_init_random(&irnd);
		std::vector<uint8_t> enc_key = encrypted_key(irnd, key);
		for (unsigned int i=0; i<key.size(); i++)
			serial.Key[i] = enc_key.at(i);
	} else {
		for (unsigned int i=0; i<key.size(); i++)
			serial.Key[i] = key.at(i);
	}
	serial.Major = m_ui->hwBox_major->value();
	serial.Minor = m_ui->hwBox_minor->value();
	serial.Release = m_ui->hwBox_release->value();
	result_t result = devinterface->set_serial_number(&serial);
	
	devinterface->cs->setUpdatesEnabled(saved_state);
	
	if(result == result_ok){
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
		infoBox.setText("Serial number updating error.\nReturned value: " + error_code);
		infoBox.exec();
	}
}

void memxor (void *dest, const void *src, size_t n)
{
	char const *s = (char const*)src;
	char *d = (char *)dest;

	for (; n > 0; n--)
		*d++ ^= *s++;
}

void PageStep3Wgt::OnSetKeyBtnPressed()
{
	result_t result;
	Base16to256(m_ui->keyBox->text(), &key);

	bool saved_state = devinterface->cs->updatesEnabled();
	devinterface->cs->setUpdatesEnabled(false);

	InfoBox infoBox;
	infoBox.setButtonsVisible(false);
	infoBox.setMovieVisible(true);
	infoBox.setIcon(QMessageBox::Information);
	infoBox.setText("Prepare for key update");
	infoBox.show();

	sleep_act(500);

	devinterface->close_device();
	infoBox.setText("Please wait while key is updating");
	sleep_act(250);

	// If bootloader version is equal to or newer than 2.3.0 then it means it needs IRND call
	if ( is_asc_order(boot_need_encrypted_key_version, controllerStgs->bootloader_version) ) {
		init_random_t irnd;
		devinterface->open_device(updateThread->device_name);
		devinterface->get_init_random(&irnd);
		devinterface->close_device();
		std::vector<uint8_t> enc_key = encrypted_key(irnd, key);
		result = devinterface->write_key(updateThread->device_name, &enc_key[0]);
	} else { // Older bootloaders get the key directly
		result = devinterface->write_key(updateThread->device_name, &key[0]);
	}

	//подхватываем девайс
	sleep_act(2*BOOTLOADER_DELAY);

	updateThread->waitForReconnect(5000);
	devinterface->cs->setUpdatesEnabled(saved_state);

	if(result == result_ok){
		infoBox.setIcon(QMessageBox::Information);
		infoBox.setButtons(QDialogButtonBox::Ok);
		infoBox.setMovieVisible(false);
		infoBox.setText("Key was updated succesfully");
		infoBox.exec();
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
		infoBox.setText("Key updating error.\nReturned value: " + error_code);
		infoBox.exec();
	}
}

void PageStep3Wgt::OnRebootToBoot()
{
	devinterface->service_command_updf();
	devinterface->close_device();
}

void PageStep3Wgt::SetKey(QString text)
{
   
	m_ui->keyBox->setText(text);
	Base16to256(text, &key);
}

QString PageStep3Wgt::GetKey()
{
	QString a;
	Base256to16(key, &a); 
	return a;
}

void PageStep3Wgt::FromUiToClass()
{
	Base16to256(m_ui->keyBox->text(), &key);
}

std::vector<uint8_t> PageStep3Wgt::encrypted_key(libximc::init_random_t irnd, std::vector<uint8_t> key)
{
		const int WKEY_SIZE = 32;
		uint8_t cyph_key[WKEY_SIZE];
		memcpy(cyph_key, &key.at(0), WKEY_SIZE);

		const int AES_BLOCK_SIZE = 16;
		uint8_t enc_iv[AES_BLOCK_SIZE];

		uint8_t irnd_key[AES_BLOCK_SIZE];
		for (int i=0; i<AES_BLOCK_SIZE; i++) // assumes irnd.key size equals AES_BLOCK_SIZE and equals 16
			irnd_key[i] = irnd.key[i];

		// Just in case this code/data ends up in executable
#ifdef SERVICEMODE
		memcpy(enc_iv, "m2c(Knd9YE 2-q;~", AES_BLOCK_SIZE);
#else
		memset(enc_iv, 0x00, AES_BLOCK_SIZE); // usermode doesn't need it anyway
#endif

		// Simulate two rounds of AES-CBC with our AES-ECB

		memxor(enc_iv, &key.at(0), AES_BLOCK_SIZE);
		AES128_ECB_encrypt(enc_iv, irnd_key, cyph_key);

		uint8_t interm[AES_BLOCK_SIZE];
		memcpy(interm, cyph_key, AES_BLOCK_SIZE);

		memxor(interm, &key.at(AES_BLOCK_SIZE), AES_BLOCK_SIZE);
		AES128_ECB_encrypt(interm, irnd_key, cyph_key+AES_BLOCK_SIZE);

		std::vector<uint8_t> encr_key(WKEY_SIZE);
		for (int i=0; i<WKEY_SIZE; ++i) {
			encr_key.at(i) = cyph_key[i];
		}

		return encr_key;
}
