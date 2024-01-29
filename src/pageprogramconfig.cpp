#include "pageprogramconfig.h"
#include "ui_pageprogramconfig.h"
#include "devicesearchsettings.h"
#include <QHostAddress>
#include <QDialog>
#include <QLineEdit>
#include <QMessageBox>
#include "functions.h"

#if defined(_MSC_VER)
 #include <winsock2.h>
#else
 #include <arpa/inet.h>
#endif

#define FUTURE_WAIT_MS 2000

QString ProtocolSelectionStyle = { //"border: 1px solid gray;"
"border - radius: 2px;"
//"padding: 1px 18px 1px 3px;"
//"min - width: 6em;" 
};


PageProgramConfigWgt::PageProgramConfigWgt(QWidget *parent, DeviceSearchSettings* _dss) :
    QWidget(parent),
    ui(new Ui::PageProgramConfigWgt)
{
    ui->setupUi(this);

	dss = _dss;
	x_icon.addFile(":/settingsdlg/images/settingsdlg/x.png");
	w_icon.addFile(":/settingsdlg/images/settingsdlg/wrench.png");
	connect( ui->detectBtn, SIGNAL( clicked() ), this, SLOT( DetectHosts() ) );
	connect( ui->tableWidget, SIGNAL( cellClicked (int, int) ), this, SLOT( slotCellClicked( int, int ) ) );
	connect( ui->tableWidget, SIGNAL( cellChanged (int, int) ), this, SLOT( slotCellChanged( int, int ) ) );

	/*connect(ui->pushButton_toRight, SIGNAL(clicked()), this, SLOT(copyUsersLocalToRemote()));
	connect(ui->pushButton_toLeft, SIGNAL(clicked()), this, SLOT(copyUsersRemoteToLocal()));
	connect(ui->pushButton_trashLocal, SIGNAL(clicked()), this, SLOT(deleteUsersLocal()));
	connect(ui->pushButton_trashRemote, SIGNAL(clicked()), this, SLOT(deleteUsersRemote()));
	connect(ui->pushButton_newLocal, SIGNAL(clicked()), this, SLOT(createUserLocal()));
	connect(ui->pushButton_newRemote, SIGNAL(clicked()), this, SLOT(createUserRemote()));*/
	fixGrayout(ui->networkChk);

	//ui->localUserTableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	//ui->remoteUserTableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	srand(time(0));

	// No servers found by default
	this->displayStatusServerCount(0);
}

PageProgramConfigWgt::~PageProgramConfigWgt()
{
    delete ui;
}

void PageProgramConfigWgt::DetectHosts()
{
	QPushButton *btn = ui->detectBtn;
	QString btn_text = btn->text();
	// displayStatus("Detecting...");
	ui->detectBtn->setText("Detecting...");
	ui->detectBtn->setEnabled(false);

	QApplication::processEvents();

	libximc::device_enumeration_t tmp_enum;
	libximc::device_network_information_t netinfo;

	tmp_enum = libximc::enumerate_devices(ENUMERATE_PROBE | ENUMERATE_NETWORK, "addr=");
	if (tmp_enum == 0) {
		displayStatus("Failure in enumerate_devices.");
		return;
	}
	int count = libximc::get_device_count(tmp_enum);
	std::vector<QString> v;

	// get information about servers from each controller network enumerate info
	for (int i=0; i<count; i++) {
		result_t result = libximc::get_enumerate_device_network_information(tmp_enum, i, &netinfo);
		if (result == result_ok && netinfo.ipv4 != 0) {  // ignore all-zero address coming from empty memory of virtual controller
			v.push_back(QHostAddress(ntohl(netinfo.ipv4)).toString());
		}
		else {
			qDebug() << "ignoring failed enumerate";
		}
	}
	// count unique found servers and save it
	std::sort(v.begin(), v.end()); 
    v.erase(std::unique(v.begin(), v.end()), v.end());
	size_t v_size = v.size();

	// add rows from the table
	for (int i=0; i<ui->tableWidget->rowCount()-1; i++) { // last row is edit row which has no items
		v.push_back( ui->tableWidget->item(i, 1)->text() );
	}
	// once more sort and clear dupes
	std::sort(v.begin(), v.end()); 
    v.erase(std::unique(v.begin(), v.end()), v.end());
	
	QList<QString> list = QList<QString>::fromVector( QVector<QString>::fromStdVector(v) );

	SetTable(list, dss->Protocol_list);

	libximc::free_enumerate_devices(tmp_enum);
	displayStatusServerCount(v_size);

	ui->detectBtn->setText(btn_text);
	ui->detectBtn->setEnabled(true);
}

void PageProgramConfigWgt::FromUiToClass()
{
	dss->Enumerate_probe = ui->ProbeDevBox->isChecked();
	dss->Enumerate_all_com = ui->NonXimcBox->isChecked();
	dss->Enumerate_network = ui->networkChk->isChecked();
	dss->Virtual_devices = ui->virtualDevicesSpinBox->value();

	dss->Server_hosts.clear();
	dss->Protocol_list.clear();
	
	for (int i = 0; i < ui->tableWidget->rowCount()-1; ++i) { // last row is edit row which has no items
		dss->Server_hosts.append(ui->tableWidget->item(i,1)->text() );
		QComboBox *boxProtocol;		
		boxProtocol = qobject_cast<QComboBox*>(
			ui->tableWidget->cellWidget(i, 0));
		dss->Protocol_list.append(/*ui->tableWidget->item(i, 0) == 0 ? QString(" ") : *//*ui->tableWidget->item(i, 0)->text()*/boxProtocol->currentText());
		//boxProtocol->setStyleSheet(ProtocolSelectionStyle);		
	}
}

void PageProgramConfigWgt::FromClassToUi()
{
	ui->ProbeDevBox->setChecked(dss->Enumerate_probe);
	ui->NonXimcBox->setChecked(dss->Enumerate_all_com);
	ui->networkChk->setChecked(dss->Enumerate_network);
	ui->virtualDevicesSpinBox->setValue(dss->Virtual_devices);

	SetTable(dss->Server_hosts, dss->Protocol_list);
}

void PageProgramConfigWgt::FromUiToSettings()
{
	FromUiToClass();
	dss->save();
}

void PageProgramConfigWgt::FromSettingsToUi()
{
	dss->load();
	FromClassToUi();
}

void PageProgramConfigWgt::displayStatus (QString text)
{
	ui->infoLabel->setText(text);
}

void PageProgramConfigWgt::displayStatusServerCount(uint64_t count)
{
	displayStatus(QString("%1 server(s) found.").arg(count));
}

void PageProgramConfigWgt::slotCellClicked ( int row, int column )
{
	if (column == 2 && row != ui->tableWidget->rowCount()-1)
		ui->tableWidget->removeRow(row);

	if (column == 3 && row != ui->tableWidget->rowCount()-1) {
		// First clear the old models so that lists become greyed out
		//delete ui->localUserTableView->model();
		//delete ui->remoteUserTableView->model();

		// Reconnect to specified address
		std::string address = ui->tableWidget->item(row, 1)->text().toStdString();
	}
}

void PageProgramConfigWgt::slotCellChanged ( int row, int column )
{
	QTableWidget *tw = ui->tableWidget;
	tw->blockSignals(true);
	if (column == 1 && row == tw->rowCount()-1 && tw->item(row, column)->text().trimmed() != QString("") ) { // add new line
		// Increase table size by one row
		tw->setRowCount(tw->rowCount() + 1);

		// Trim spaces in edited line (left column)
		tw->item(row, 1)->setText(tw->item(row, 1)->text().trimmed());

		// Add "x" icon in the edited row (next column)
		QTableWidgetItem* icon_item = new QTableWidgetItem();
		icon_item->setIcon(x_icon);
		icon_item->setFlags(Qt::ItemIsEnabled);
		tw->setItem(row, 2, icon_item);

		// Add "wrench" icon in the edited row (right column)
		QTableWidgetItem* icon_item2 = new QTableWidgetItem();
		icon_item2->setIcon(w_icon);
		icon_item2->setFlags(Qt::ItemIsEnabled);
		tw->setItem(row, 3, icon_item2);

		// Disable right column of the new row
		tw->setItem(row+1, 2, new QTableWidgetItem());
		tw->item(row+1, 2)->setFlags(Qt::NoItemFlags);

		QComboBox *boxProtocol = new QComboBox;
		boxProtocol->setStyleSheet(ProtocolSelectionStyle);

		//�� ����������� ��������, �� ��������� - ��� ����� ��������� :)
		boxProtocol->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		boxProtocol->addItem(XI_NET);
		boxProtocol->addItem(XI_UDP);
		boxProtocol->addItem(XI_TCP);
		
		//boxProtocol->setCurrentIndex(0);
		//��������� � ������� QTableWidget � ������� �0
		ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, boxProtocol);
	} else if (column == 1 && row != tw->rowCount()-1 && tw->item(row, column)->text().trimmed() == QString("") ) { // remove empty line
		tw->removeRow(row);
	}
	tw->blockSignals(false);
}

void PageProgramConfigWgt::SetTable(QList<QString> list, QList<QString> list_protocol)
{
	QTableWidget *tw = ui->tableWidget;

	tw->blockSignals(true);

	tw->clearContents();
	tw->setRowCount(list.size()+1);
	tw->setColumnCount(4);

	QStringList qslist;
	qslist << QString("Protocol") << QString("IP/Host[:port]") << QString() << QString() ;
	ui->tableWidget->setHorizontalHeaderLabels(qslist);	
	QHeaderView * h = ui->tableWidget->horizontalHeader();
	QHeaderView * v = ui->tableWidget->verticalHeader();
	h->setClickable(false);
	h->setResizeMode(1, QHeaderView::Stretch);
	h->setResizeMode(0, QHeaderView::ResizeToContents);
	h->setResizeMode(2, QHeaderView::ResizeToContents);
	h->setResizeMode(3, QHeaderView::ResizeToContents);
	v->hide();

	for (int i=0; i<list.size(); ++i) {
		tw->setItem(i, 1, new QTableWidgetItem( list.at(i) ) );
		tw->setItem(i, 0, new QTableWidgetItem( list_protocol.at(i) ) );
		QComboBox *boxProtocol = new QComboBox;
		boxProtocol->setStyleSheet(ProtocolSelectionStyle);
		int curr_ind;
		//�� ����������� ��������, �� ��������� - ��� ����� ��������� :)
		boxProtocol->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		boxProtocol->addItem(XI_NET);
		boxProtocol->addItem(XI_UDP);
		boxProtocol->addItem(XI_TCP);
		if (list_protocol.at(i) == XI_UDP)
			curr_ind = 1;
		else
		if (list_protocol.at(i) == XI_TCP)
				curr_ind = 2;
			else
				curr_ind = 0;

		boxProtocol->setCurrentIndex(curr_ind);
		//��������� � ������� QTableWidget � ������� �0
		ui->tableWidget->setCellWidget(i, 0, boxProtocol);

		QTableWidgetItem* icon_item = new QTableWidgetItem();
		icon_item->setIcon(x_icon);
		icon_item->setFlags(Qt::ItemIsEnabled);
		tw->setItem(i, 2, icon_item );

		QTableWidgetItem* icon_item2 = new QTableWidgetItem();
		icon_item2->setIcon(w_icon);
		icon_item2->setFlags(Qt::ItemIsEnabled);
		tw->setItem(i, 3, icon_item2 );
	}

	QComboBox *boxProtocol = new QComboBox;
	boxProtocol->setStyleSheet(ProtocolSelectionStyle);

	//�� ����������� ��������, �� ��������� - ��� ����� ��������� :)
	boxProtocol->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	boxProtocol->addItem(XI_NET);
	boxProtocol->addItem(XI_UDP);
	boxProtocol->addItem(XI_TCP);
	ui->tableWidget->setCellWidget(list.size(), 0, boxProtocol);

	// this is the edit line
	tw->setItem(list.size(), 2, new QTableWidgetItem());
	tw->setItem(list.size(), 3, new QTableWidgetItem());
	tw->item(list.size(), 2)->setFlags(Qt::NoItemFlags);
	tw->item(list.size(), 3)->setFlags(Qt::NoItemFlags);

	tw->blockSignals(false);
}
