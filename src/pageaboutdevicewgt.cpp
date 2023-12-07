#include <ui_pageaboutdevicewgt.h>
#include <pageaboutdevicewgt.h>
#include <messagelog.h>
//#include <WinNT.h>

using namespace tinyxml2;

extern QString xilab_ver;
const char* update_xml_addr = "http://files.mdrive.tech/products.xml";

extern const char* major;
extern const char* minor;
extern const char* release;
extern const char* version;
extern const char* UTCTime;
extern const char* SHA1;
extern const char* URL;
extern const char* ximc;

PageAboutDeviceWgt::PageAboutDeviceWgt(QWidget *parent, MotorSettings *_motorStgs, ControllerSettings *controllerStgs_loc, FirmwareUpdateThread *firmwareUpdate_loc, DeviceInterface *_devinterface, QString *_default_firmware_path)
    : QWidget(parent),
	m_ui(new Ui::PageAboutDeviceWgtClass)
{

	devinterface = _devinterface;
	controllerStgs_local = controllerStgs_loc;
	motorStgs = _motorStgs;
	default_firmware_path = _default_firmware_path;
	choosefirmwaredlg = new ChooseFirmware(QString(update_xml_addr));
	choosefirmwaredlg->setWindowFlags(Qt::WindowMaximizeButtonHint | Qt::Tool);
	m_ui->setupUi(this);

	connect(m_ui->updateFirmwareBtn,	SIGNAL(clicked()),  this,  SLOT(OnUpdateFirmwareBtnClicked()));
	connect(m_ui->updateFirmwareInternetBtn, SIGNAL(clicked()), this, SLOT(OnUpdateFirmwareInternetBtnClicked()));
	connect(m_ui->eepromPrecedenceBox,	SIGNAL(toggled(bool)),  this,  SIGNAL(precedenceCheckedSgn(bool)));
	connect(m_ui->chooseFirmware, SIGNAL(clicked()), this, SLOT(CallChooseFirmDlg()));
	connect(choosefirmwaredlg, SIGNAL(clicked(QString, QString)), this, SLOT(updateChooseFirmware(QString, QString)));
	connect(choosefirmwaredlg, SIGNAL(updateList(QUrl)), this, SLOT(RetryInternetConn(QUrl)));
	connect(this, SIGNAL(networkResultSgn(QString, bool, int)), choosefirmwaredlg, SLOT(showMessageAndClose(QString, bool, int)));
	connect(m_ui->TechSupport, SIGNAL(clicked()), this, SLOT(OnTechSupportClicked()));
	
	firmwareUpdate_local = firmwareUpdate_loc;
	connect(firmwareUpdate_local,	SIGNAL(update_finished(result_t)),
									SLOT(OnUpdateFirmwareFinished(result_t)));

	manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotFinished(QNetworkReply*)));
	QNetworkRequest request;
	request.setUrl(QUrl(update_xml_addr));
	request.setRawHeader("User-Agent", QString("mDrive Direct Control %1").arg(xilab_ver).toUtf8());
	manager->get(request);
	m_ui->progressBar->hide();
	m_ui->doc_label->setOpenExternalLinks(true);
	m_ui->prog_label->setOpenExternalLinks(true);
}

PageAboutDeviceWgt::~PageAboutDeviceWgt()
{
	delete m_ui;
}

QString cacheFileNameFromString(QString hash_string)
{
	return getDefaultPath() + "/" + hash_string; // todo: if nested dirs then need to mkdir elsewhere
}

QString cacheFileNameUpdateXml()
{
	return cacheFileNameFromString("update.xml");
}

void PageAboutDeviceWgt::defaultLocationChanged(bool isDefault)
{
	isDefaultLocation = isDefault;
}

void PageAboutDeviceWgt::FromClassToUi()
{
	m_ui->snEdit->setText(toStr(controllerStgs_local->serialnumber));
	m_ui->snEdit->setTextInteractionFlags(Qt::TextSelectableByMouse);
	QString strHarVer;

	// Added to fix the iron version for the erroneous iron version 1.3.1
	if (controllerStgs_local->device_info.Major == 1 && controllerStgs_local->device_info.Minor == 3 && controllerStgs_local->device_info.Release == 1)
		strHarVer = "2.3.3";
	else
		strHarVer = QString::number(controllerStgs_local->device_info.Major) + "." + QString::number(controllerStgs_local->device_info.Minor) + "." + QString::number(controllerStgs_local->device_info.Release);


	m_ui->hardwVer->setText(strHarVer);
	m_ui->hardwVer->setTextInteractionFlags(Qt::TextSelectableByMouse);
	QString strBootVer = QString::number(controllerStgs_local->bootloader_version.major)+"."+QString::number(controllerStgs_local->bootloader_version.minor)+"."+QString::number(controllerStgs_local->bootloader_version.release);
	m_ui->blVer->setText(strBootVer);
	m_ui->blVer->setTextInteractionFlags(Qt::TextSelectableByMouse);

	QString strFirmVer = QString::number(controllerStgs_local->firmware_version.major)+"."+QString::number(controllerStgs_local->firmware_version.minor)+"."+QString::number(controllerStgs_local->firmware_version.release);
	m_ui->firmVer->setText(strFirmVer);
    m_ui->firmVer->setTextInteractionFlags(Qt::TextSelectableByMouse);

	m_ui->LfirmVer_2->setTextInteractionFlags(Qt::TextSelectableByMouse);

	m_ui->manufacturerEdit->setText(controllerStgs_local->device_info.Manufacturer);
	m_ui->manidEdit->setText(controllerStgs_local->device_info.ManufacturerId);
	m_ui->descEdit->setText(controllerStgs_local->device_info.ProductDescription);

	m_ui->friendlyName->setText(QString::fromUtf8(motorStgs->name.ControllerName));
	m_ui->eepromPrecedenceBox->setChecked(motorStgs->name.CtrlFlags & EEPROM_PRECEDENCE);
}

void PageAboutDeviceWgt::FromUiToClass(MotorSettings *lmStgs)
{
	lmStgs->name.CtrlFlags = 0;

	safe_copy(lmStgs->name.ControllerName, m_ui->friendlyName->text().toUtf8().data());
	setUnsetBit(m_ui->eepromPrecedenceBox->isChecked(), &lmStgs->name.CtrlFlags, EEPROM_PRECEDENCE);
}

void PageAboutDeviceWgt::FwUpdateRoutine(QByteArray data)
{
	firmwareUpdate_local->fw_data = data;
	firmwareUpdate_local->start();

	//messagebox setup&exec
	infoBox.setIcon(QMessageBox::Information);
	infoBox.setButtonsVisible(false);
	infoBox.setMovieVisible(true);
	infoBox.setText("Please wait while firmware is updating");
	infoBox.show();
}

void PageAboutDeviceWgt::OnUpdateFirmwareBtnClicked()
{
	if (this->devinterface->getProtocolType() != dtSerial)
	{
		QMessageBox::warning(this, "Error", "You cannot update the firmware via Ethernet. To update the firmware, please use a USB connection.", QMessageBox::Ok);
		return;
	}
	QString filename, firmware_path;
	if((*default_firmware_path == "") || (isDefaultLocation))
		firmware_path = getDefaultPath();
	else
		firmware_path = *default_firmware_path;

	filename = QFileDialog::getOpenFileName(this, tr("Open firmware file"), firmware_path, tr("Firmware files (*.cod)"));
	if(!filename.isEmpty()){
		QApplication::processEvents();

		QFile file(filename);
		if (file.exists()) {
			file.open(QIODevice::ReadOnly);
			QByteArray data(file.readAll());
			file.close();
			FwUpdateRoutine(data);
		}

		//Ð¾Ð±Ð½Ð¾Ð²Ð»ÑÐµÐ¼ Ð´ÐµÑ„Ð¾Ð»Ñ‚Ð½Ñ‹Ðµ Ð¿ÑƒÑ‚Ð¸
		QDir dir;
		dir.setPath(filename);
		dir.cdUp();
		*default_firmware_path = dir.path();
	}
}


QString PageAboutDeviceWgt::getServiceMailInfo(const QString line_sep)
{
	QString service_info;
	service_info.append(" >> mDrive Direct Control version -- ");
	service_info.append(xilab_ver);

	result_t result;
	libximc::device_information_t inf1;
	service_info.append(line_sep);
	service_info.append(" >> Hardware version -- ");
	result = devinterface->get_device_information(&inf1);
	service_info.append(QString::number(inf1.Major) + "." + QString::number(inf1.Minor) + "." + QString::number(inf1.Release));

	service_info.append(line_sep);
	service_info.append(" >> Firmware version -- ");
	QString data1;
	data1 = m_ui->firmVer->text();
	service_info.append(data1);

	service_info.append(line_sep);
	service_info.append(" >> Library version -- ");
	char version11[32];
	libximc::ximc_version(version11);
	service_info.append(version11);

	service_info.append(line_sep);
	service_info.append(" >> Serial number -- ");
	data1 = m_ui->snEdit->text();
	service_info.append(data1);

	status_t state1;
	result = devinterface->get_status(&state1);

	if ((result == result_ok) && (state1.Flags & STATE_EEPROM_CONNECTED))
	{
		service_info.append(line_sep);
		service_info.append(" >> EEPROM connect ");
		stage_information_t stage_inf;
		result = devinterface->get_stage_information(&stage_inf);
		if (result == result_ok)
		{
			service_info.append(line_sep);
			service_info.append(" >> Part number -- ");
			service_info.append(stage_inf.PartNumber);
		}
	}
	else
	{
		service_info.append(line_sep);
		service_info.append(" >> EEPROM no connect   ");
	}
	return service_info;
}


void PageAboutDeviceWgt::OnTechSupportClicked()
{
	QString service_info;
#if defined(__LINUX__) || defined(__APPLE__)
	QString sep = "\r\n";
#endif
#if defined(WIN32) || defined(WIN64)
	
	QString sep = "%0D%0A";

	if (QSysInfo::WindowsVersion >= QSysInfo::WV_WINDOWS8/*160*/) {
		QWhatsThis::showText(QPoint(cursor().pos().x(), cursor().pos().y()), "You will be directed to the technical support page.");
		QDesktopServices::openUrl(QUrl("https://physlab.ru/contacts", QUrl::TolerantMode));
	}
	else
#endif
	{
		QString mail_data;
		mail_data = "mailto:support@mdrive.tech?subject= &body=<The text of the letter>";
		mail_data.append(sep);
		mail_data.append(sep);
		mail_data.append("Service information ");
		mail_data.append(sep);
		mail_data.append(this->getServiceMailInfo(sep));
		QWhatsThis::showText(QPoint(cursor().pos().x(), cursor().pos().y()), "Opens the mail client if it is installed.");
		QDesktopServices::openUrl(QUrl(mail_data.toUtf8(), QUrl::TolerantMode));
	}
}

void PageAboutDeviceWgt::OnUpdateFirmwareFinished(result_t result)
{
	infoBox.setButtons(QDialogButtonBox::Ok);
	infoBox.setMovieVisible(false);

	if (result != result_ok) {
		QString error_code;
		switch (result) {
			case result_error: error_code = "generic error"; break;
			case result_not_implemented: error_code = "function is not implemented"; break;
			case result_nodevice: error_code = "device is lost"; break;
			default: error_code = "microcode file not found"; break;
		}
		infoBox.setIcon(QMessageBox::Critical);
		infoBox.setText("Firmware update error.\nReturned value: " + error_code);
	}
	else{
		FromClassToUi();
		infoBox.setIcon(QMessageBox::Information);
		infoBox.setText("Firmware updated succesfully.");
	}	
}

void PageAboutDeviceWgt::OnUpdateSerialFinished()
{
	FromClassToUi();
}

void PageAboutDeviceWgt::slotReadyRead()
{

}

void PageAboutDeviceWgt::updateChooseFirmware(QString url, QString hash)
{
	if (this->devinterface->getProtocolType() != dtSerial)
	{
		QMessageBox::warning(this, "Error", "You cannot update the firmware via Ethernet. To update the firmware, please use a USB connection.", QMessageBox::Ok);
		return;
	}
	hash_string = hash;
	RetryInternetConn(QUrl(url));
}

void PageAboutDeviceWgt::RetryInternetConn(QUrl resource_address)
{
	showProgress();
	QNetworkRequest request;
    request.setUrl(resource_address);
	request.setRawHeader("User-Agent", QString("mDrive Direct Control %1").arg(xilab_ver).toUtf8()); // todo : check how to get i10n urls correctly
	reply = manager->get(request);
}

void PageAboutDeviceWgt::slotFinished(QNetworkReply* reply)
{
	hideProgress();

	QByteArray bytes = reply->readAll();
	QString string_url =reply->request().url().toString(); 

	if(bytes == "") {
		emit networkResultSgn("Internet connection error, firmware update service is disabled. Motion control will not be affected.", false, LOGLEVEL_WARNING);
		return;
	}
	else if (reply->request().url() == QUrl(update_xml_addr)) { // we got a list of firmwares !
		QFile file( cacheFileNameUpdateXml() );
		file.open(QIODevice::WriteOnly);
		file.write(bytes);
		file.flush();
		file.close();

		tinyxml2::XMLDocument doc;
		doc.LoadFile( cacheFileNameUpdateXml().toUtf8().data() );
		// Validate XML
		if (doc.Error()){
			disconnect(reply);
			reply->deleteLater();
			emit networkResultSgn("XML validation error.", false, LOGLEVEL_ERROR);
			return;
		}

		XMLElement* Firmware = doc.RootElement()->FirstChildElement( ximc );
		XMLElement* element = Firmware;
		leaf_element_t max_allowed_firmware;
		hardware_version_t hw_ver;

		// Added to fix the iron version for the erroneous iron version 1.3.1
		if (controllerStgs_local->device_info.Major == 1 && controllerStgs_local->device_info.Minor == 3 && controllerStgs_local->device_info.Release == 1)
		{
			hw_ver.major = 2;
			hw_ver.minor = 3;
			hw_ver.release = 3;
		}
		else
		{		
		hw_ver.major = controllerStgs_local->device_info.Major;
		hw_ver.minor = controllerStgs_local->device_info.Minor;
		hw_ver.release = controllerStgs_local->device_info.Release;
		}
		choosefirmwaredlg->fill_treeWdg(element, &max_allowed_firmware, hw_ver);//fill window with firmwares, find max allowed firmware for the given hardware version
		emit networkResultSgn(" ", false, LOGLEVEL_DEBUG);

		if (max_allowed_firmware.valid) {
			version_t v = max_allowed_firmware.version;
			QString strLFirmVer = QString("%1.%2.%3").arg(v.major).arg(v.minor).arg(v.release);
			m_ui->LfirmVer_2->setText(strLFirmVer);
			firmware_url = max_allowed_firmware.URL.toString();
			hash_string = max_allowed_firmware.SHA1_string;
		}


	} else { // we're dealing with a binary blob already
		QCryptographicHash hash(QCryptographicHash::Sha1);
		hash.addData(bytes);
		if (QString(hash.result().toHex()) != hash_string) { // fail for some reason
			emit networkResultSgn(QString("Firmware hash validation error"), true, LOGLEVEL_WARNING);
		} else {
			emit networkResultSgn("Firmware file download OK", true, LOGLEVEL_INFO); // erase the label text, close the window and update firmare
		}
		QApplication::processEvents(); // #7095: force process signal queue before spawning another modal window in FwUpdateRoutine to fix flashing bug on linux
		FwUpdateRoutine(bytes);
	}

	disconnect(reply);
	reply->deleteLater();
}

void PageAboutDeviceWgt::slotProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	if (bytesTotal){
		m_ui->progressBar->setValue(100 * bytesReceived / bytesTotal);
	}
	else 
		hideProgress();
}

void PageAboutDeviceWgt::showProgress()
{
	download_start_time = QDateTime::currentDateTimeUtc();
	m_ui->progressBar->show();
}

void PageAboutDeviceWgt::hideProgress()
{
	qint64 elapsed = download_start_time.msecsTo(QDateTime::currentDateTimeUtc());
	QTimer::singleShot(1000-elapsed, this, SLOT(hideProgressHelper()));
}
	
void PageAboutDeviceWgt::hideProgressHelper()
{
	m_ui->progressBar->hide();
}


void PageAboutDeviceWgt::slotError(QNetworkReply::NetworkError error)
{
	Q_UNUSED(error)
	disconnect(reply);
	emit errorInLog("Error downloading firmware: cannot connect to the update server.", SOURCE_XILAB, LOGLEVEL_ERROR);
}

void PageAboutDeviceWgt::OnUpdateFirmwareInternetBtnClicked()
{
		if (this->devinterface->getProtocolType() != dtSerial)
		{
			QMessageBox::warning(this, "Error", "Cannot auto-update firmware via Ethernet.\nUse USB connection.", QMessageBox::Ok);
			return;
		}
		showProgress();
		m_ui->progressBar->setValue(0);
		//get information about latest firmware
		QUrl base = QUrl(update_xml_addr);
		RetryInternetConn(base);
		//wait first request
		QEventLoop loop;
		connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
		loop.exec();
		//download firmware
		RetryInternetConn(firmware_url);

		connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
		connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
		connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotProgress(qint64, qint64)) );
}

void PageAboutDeviceWgt::CallChooseFirmDlg(){

	choosefirmwaredlg->show();
}
