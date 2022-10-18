#include "choosefirmware.h"
#include "ui_choosefirmware.h"
#include "functions.h"
using namespace tinyxml2;

const char* major = "major";
const char* minor = "minor";
const char* release  = "release";
const char* version = "version";
const char* UTCTime = "utc";
const char* SHA1 = "sha1";
const char* URL = "url";
const char* separator = ".";
const char* ximc = "ximc";
const int URL_DATA_COLUMN = 0;
const int SHA_DATA_COLUMN = 1;

ChooseFirmware::ChooseFirmware(QString _xml_url, QWidget* parent) :
QDialog(parent,Qt::CustomizeWindowHint| Qt::WindowCloseButtonHint |Qt::MSWindowsFixedSizeDialogHint),
		ui(new Ui::ChooseFirmware)
{
	xml_url = _xml_url;
    ui->setupUi(this);
    ui->treeWidget->header()->hide();
	connect(ui->updateBtn_2,SIGNAL(clicked()),this,SLOT(checkClk()));
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),this, SLOT(get_double_clk_item(QTreeWidgetItem*, int)));
	connect(ui->refreshBtn_2,SIGNAL(clicked()),this,SLOT(up_list()));
	ui->statusLabel->setText("");
}

ChooseFirmware::~ChooseFirmware()
{
    delete ui;
}

void ChooseFirmware::fill_treeWdg(XMLElement* root_element, leaf_element_t *max_allowed, hardware_version_t hw_ver) {
	ui->treeWidget->clear();

	int major_value = 0;
	int minor_value = 0;
	int release_value = 0;
	int level1_count = 0;
	int level2_count = 0;
	int level3_count = 0;
	XMLElement* major_element = NULL;
	XMLElement* minor_element = NULL;
	XMLElement* release_element = NULL;
	XMLElement* firmware_element = NULL;
	max_allowed->valid = false; // by default we couldn't find any valid element
	max_allowed->version = { 0, 0, 0 }; // init with lowest possible version
	leaf_element_t tmp_max;
	QString any = "x";
	QRegExp hw("hw(\\d+|x).(\\d+|x).(\\d+|x).cod");

	if (root_element != NULL) {
		major_element = root_element->FirstChildElement(major);
	}
	while (major_element != NULL) {
		if (XML_SUCCESS != major_element->QueryIntAttribute( version, &major_value ))
			break;

		minor_element = major_element->FirstChildElement(minor);
		while (minor_element != NULL){
			level3_count = 0;
			if (XML_SUCCESS != minor_element->QueryIntAttribute( version, &minor_value ))
				break;

			release_element = minor_element->FirstChildElement(release);

			QTreeWidgetItem *treenode = new QTreeWidgetItem(ui->treeWidget);
			QString major_minor = QString::number(major_value) + separator + QString::number(minor_value);
			treenode->setText(level3_count, major_minor);
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
					XMLElement* e = firmware_element->FirstChildElement(UTCTime);
				
					if (XML_SUCCESS != release_element->QueryIntAttribute( version, &release_value ))
						break;
					if (e == NULL)
						break;

					QTreeWidgetItem *treeleaf = new QTreeWidgetItem(treenode);
					QString major_minor_release = major_minor + separator + QString::number(release_value);
					treeleaf->setText(0, major_minor_release);

					QString comment(e->GetText());
					unsigned int unix_time = comment.toUInt();
					QDateTime time_reading = QDateTime::fromTime_t(unix_time);
					treeleaf->setToolTip(0, time_reading.toString());

					tmp_max.UTCTime = time_reading;
					tmp_max.version.major = major_value;
					tmp_max.version.minor = minor_value;
					tmp_max.version.release = release_value;
					e = firmware_element->FirstChildElement(SHA1);
					if (!e) {
						break;
					}
					tmp_max.SHA1_string = e->GetText();
					e = firmware_element->FirstChildElement(URL);
					if (!e) {
						break;
					}
					tmp_max.URL = QUrl(e->GetText());
					tmp_max.valid = true;
					treeleaf->setData(URL_DATA_COLUMN, Qt::UserRole, tmp_max.URL);
					treeleaf->setData(SHA_DATA_COLUMN, Qt::UserRole, tmp_max.SHA1_string);

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
				release_element = release_element->NextSiblingElement(release);
			}
			minor_element = minor_element->NextSiblingElement(minor);
			level2_count++;
		}
		major_element = major_element->NextSiblingElement(major);
		level1_count++;
	}

	// Clean up major_minor branches with no release leaves
	for (int i = ui->treeWidget->topLevelItemCount()-1; i >= 0; --i)
	{
		QTreeWidgetItem* item = ui->treeWidget->topLevelItem(i);
		if (item->childCount() == 0) {
			delete item;
		}
	}
}

void ChooseFirmware::get_double_clk_item(QTreeWidgetItem* Item, int col) {
	Q_UNUSED(col)
	emit clicked(Item->data(URL_DATA_COLUMN, Qt::UserRole).toString(),
		         Item->data(SHA_DATA_COLUMN, Qt::UserRole).toString());
}

void ChooseFirmware::up_list(){
	showMessageAndClose("Updating firmware list from server...", false);
	QUrl up_addr(xml_url);
	emit updateList(up_addr);
}

void ChooseFirmware::checkClk(){
	if (ui->treeWidget->selectedItems().length() > 0) {
		showMessageAndClose("Downloading firmware...", false);
		emit clicked(ui->treeWidget->selectedItems().at(0)->data(URL_DATA_COLUMN, Qt::UserRole).toString(),
                     ui->treeWidget->selectedItems().at(0)->data(SHA_DATA_COLUMN, Qt::UserRole).toString());
	}
}

void ChooseFirmware::showMessageAndClose(QString message, bool shouldClose, int dummy) {
	Q_UNUSED(dummy)
	ui->statusLabel->setText(message);
	if (shouldClose) {
		this->hide();
	}
}
