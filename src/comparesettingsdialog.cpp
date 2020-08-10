#include <ui_comparesettingsdialog.h>
#include <comparesettingsdialog.h>
#include <QtGui>
#include <functions.h>
#include <xsettings.h>

CompareSettingsDialog::CompareSettingsDialog(QWidget *parent, QStringList filenames) :
	QDialog(parent),
	m_ui(new Ui::CompareSettingsDialogClass)
{
//	m_ui->setupUi(this);
//	m_ui->listWidget->addItems(items);

	XSettings settings1(filenames.at(0), QSettings::IniFormat, QIODevice::ReadOnly);
	XSettings settings2(filenames.at(1), QSettings::IniFormat, QIODevice::ReadOnly);
	QStringList keys = settings1.allKeys() + settings2.allKeys();
	qSort(keys);
	keys.removeDuplicates();

	QTableWidget *tableWidget = new QTableWidget(this);
	tableWidget->setColumnCount(3);

	QStringList headerLabels;
	headerLabels << QString("Key name") << getFileName(filenames.at(0)) << getFileName(filenames.at(1));
	tableWidget->setHorizontalHeaderLabels(headerLabels);
	int n = 0;
	Q_FOREACH (QString key, keys) {
		if (!key.contains("Window_params") && !key.contains("@")) {
			if (settings1.value(key) != settings2.value(key)) {
				tableWidget->insertRow(n);
				QTableWidgetItem *item0 = new QTableWidgetItem(key);
				item0->setFlags(Qt::ItemIsEnabled);
				tableWidget->setItem(n, 0, item0);

				QTableWidgetItem *item1 = new QTableWidgetItem("<NO KEY>");
				if (settings1.contains(key)) 
					*item1 = QTableWidgetItem(settings1.value(key).toString());
				item1->setFlags(Qt::ItemIsEnabled);
				tableWidget->setItem(n, 1, item1);

				QTableWidgetItem *item2 = new QTableWidgetItem("<NO KEY>");
				if (settings2.contains(key))
					*item2 = QTableWidgetItem(settings2.value(key).toString());
				item2->setFlags(Qt::ItemIsEnabled);
				tableWidget->setItem(n, 2, item2);
				
				n++;
			}
		}
	}
	QVBoxLayout *layout = new QVBoxLayout(this);
	if (n == 0) {
		delete tableWidget;
		QLabel *label = new QLabel(this);
		label->setText("Settings in these two files appear to be identical.");
		layout->addWidget(label);
	} else {
		tableWidget->resizeColumnsToContents();
		layout->addWidget(tableWidget);
		this->setMinimumWidth(tableWidget->horizontalHeader()->length()+100);
	}
	this->setLayout(layout);
	this->show();
	this->setMinimumWidth(0);

	this->setWindowTitle(QApplication::translate("CompareSettingsDialogClass", "Compare settings", 0, QApplication::UnicodeUTF8));
}

CompareSettingsDialog::~CompareSettingsDialog()
{
	delete m_ui;
}