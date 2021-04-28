#include "checkcompatibility.h"
#include "functions.h"

//using namespace tinyxml2;

extern QString xilab_ver;

const char* update_xml_addr_ = "http://files.ximc.ru/products2.xml";

const char* major_ = "major";
const char* minor_ = "minor";
const char* release_ = "release";
const char* version_ = "version";
const char* protocol_ = "protocol";
const char* UTCTime_ = "utc";
const char* SHA1_ = "sha1";
const char* URL_ = "url";
const char* separator_ = ".";
const char* ximc_ = "ximc";
const char* xilab_ = "xilab";
const int URL_DATA_COLUMN = 0;
const int SHA_DATA_COLUMN = 1;
hardware_version_t hw_ver_;

void search_max_allowed(XMLElement* root_element, leaf_element_t *max_allowed, hardware_version_t hw_ver) {
	
	int major_value = 0;
	int minor_value = 0;
	int release_value = 0;
	int protocol_major_value = 0;
	int protocol_minor_value = 0;
	int protocol_release_value = 0;
	int level1_count = 0;
	int level2_count = 0;
	int level3_count = 0;
	XMLElement* major_element = NULL;
	XMLElement* minor_element = NULL;
	XMLElement* release_element = NULL;
	XMLElement* firmware_element = NULL;
	XMLElement* protocol_major_element = NULL;
	XMLElement* protocol_minor_element = NULL;
	XMLElement* el = NULL;
	max_allowed->valid = false; // by default we couldn't find any valid element
	max_allowed->version = { 0, 0, 0 }; // init with lowest possible version
	//max_allowed->protocol = { 0, 0, 0 }; // init with lowest possible protocol version
	leaf_element_t tmp_max;
	QString any = "x";
	QRegExp hw("hw(\\d+|x).(\\d+|x).(\\d+|x).cod");

	if (root_element != NULL) {
		major_element = root_element->FirstChildElement(major_);
	}
	while (major_element != NULL) {
		if (XML_SUCCESS != major_element->QueryIntAttribute(version_, &major_value))
			break;

		minor_element = major_element->FirstChildElement(minor_);
		while (minor_element != NULL){
			level3_count = 0;
			if (XML_SUCCESS != minor_element->QueryIntAttribute(version_, &minor_value))
				break;

			release_element = minor_element->FirstChildElement(release_);

			QString major_minor = QString::number(major_value) + separator_ + QString::number(minor_value);

			while (release_element != NULL) {
				firmware_element = release_element->FirstChildElement();
				while (firmware_element != NULL) {
					// Compatibility check based on firmware tag name
					if (!hw.exactMatch(firmware_element->Name())) {
						firmware_element = firmware_element->NextSiblingElement();
						continue;
					}
					if ((hw.cap(1) != any && hw.cap(1).toUInt() != hw_ver.major) ||
						(hw.cap(2) != any && hw.cap(2).toUInt() != hw_ver.minor) ||
						(hw.cap(3) != any && hw.cap(3).toUInt() != hw_ver.release)) {
						firmware_element = firmware_element->NextSiblingElement();
						continue;
					}
					// Compatibility check end

					tmp_max.valid = false;
					XMLElement* e = firmware_element->FirstChildElement(UTCTime_);

					if (XML_SUCCESS != release_element->QueryIntAttribute(version_, &release_value))
						break;
					if (e == NULL)
						break;

					QString major_minor_release = major_minor + separator_ + QString::number(release_value);

					QString comment(e->GetText());
					unsigned int unix_time = comment.toUInt();
					QDateTime time_reading = QDateTime::fromTime_t(unix_time);

					tmp_max.UTCTime = time_reading;
					tmp_max.version.major = major_value;
					tmp_max.version.minor = minor_value;
					tmp_max.version.release = release_value;
					e = firmware_element->FirstChildElement(SHA1_);
					if (!e) {
						break;
					}
					tmp_max.SHA1_string = e->GetText();
					e = firmware_element->FirstChildElement(URL_);
					if (!e) {
						break;
					}
					tmp_max.URL = QUrl(e->GetText());
					tmp_max.valid = true;

					// If we found a greater version and it is an allowed one (see above), then save it
					if (is_asc_order(max_allowed->version, tmp_max.version)) {
						max_allowed->SHA1_string = tmp_max.SHA1_string;
						max_allowed->URL = tmp_max.URL;
						max_allowed->UTCTime = tmp_max.UTCTime;
						max_allowed->valid = tmp_max.valid;
						max_allowed->version = tmp_max.version;
					}
					// */

					firmware_element = firmware_element->NextSiblingElement();
				}
				level3_count++;
				release_element = release_element->NextSiblingElement(release_);
			}
			minor_element = minor_element->NextSiblingElement(minor_);
			level2_count++;
		}
		major_element = major_element->NextSiblingElement(major_);
		level1_count++;
	}

}

void  chek()
{
	QEventLoop eventLoop;
	// "quit()" the event-loop, when the network request "finished()"
	QNetworkAccessManager manager_;
	QObject::connect(&manager_, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
	
	QNetworkRequest request;
	request.setUrl(QUrl(update_xml_addr_));
	request.setRawHeader("User-Agent", QString("XiLab %1").arg(xilab_ver).toUtf8());
		
	QNetworkReply *reply = manager_.get(request);
	eventLoop.exec(); // blocks stack until "finished()" has been called
	

	tinyxml2::XMLDocument doc;
	if (reply->error() == QNetworkReply::NoError) {

		QString strReply = (QString)reply->readAll();
		MessageCompatibility(strReply);
		// XML читается, но пока не удалось передать его в XMLDocument doc;
		//strReply.toUtf8().data()
		
		XMLDocument doc;
		doc.Parse(reply->readAll());
		XMLElement* Firmware = doc.RootElement()->FirstChildElement(ximc_);
		XMLElement* element = Firmware;


		if (doc.Error()){
			leaf_element_t max_allowed_firmware_;
			hw_ver_.major = 2;
			hw_ver_.minor = 3;
			hw_ver_.release = 3;
			search_max_allowed(element, &max_allowed_firmware_, hw_ver_);
			MessageCompatibility("Error");

			delete reply;
		}
	}
	else {
		//failure
		MessageCompatibility("Error_1");
		delete reply;
	}


	
	XMLElement* Firmware = doc.RootElement()->FirstChildElement(ximc_);
	XMLElement* element = Firmware;
	leaf_element_t max_allowed_firmware;
	//	hardware_version_t hw_ver_;
	// return element;

	MessageCompatibility("zsdgzsg");

	//	reply->deleteLater();
}


void MessageCompatibility(QString message)
{
	QMessageBox msgBox;
	// Заголовок сообщения
	msgBox.setText("Warning..");
	// Тип иконки сообщения
	msgBox.setIcon(QMessageBox::Information);
	// Основное сообщение Message Box
	msgBox.setInformativeText(message);
	// Добавление реагирования на софт клавиши
	QPushButton* pButtonYes = msgBox.addButton("Ok!", QMessageBox::YesRole);
	msgBox.addButton("Remind me later", QMessageBox::NoRole);

	//msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Later);
	// На какой кнопке фокусироваться по умолчанию
	//msgBox.setDefaultButton(QMessageBox::Ok);
	/* Запускаем QMessageBox. После выполнения, в ret будет лежать значение кнопки, на которую нажали - это необходимо для дальнейшей обработки событий*/
	int ret = msgBox.exec();
	// Собственно вот этот case и отвечает за обработку событий
	switch (ret) {
	case QMessageBox::NoRole:
		// Сюда пишем обработку события Cancel
		break;
	case QMessageBox::YesRole:
		// Сюда пишем обработку события Ok
		break;
	default:
		// Сюда пишем обработку события по умолчанию
		break;
	}
}

