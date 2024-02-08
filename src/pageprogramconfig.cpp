#include "pageprogramconfig.h"
#include "ui_pageprogramconfig.h"
#include "devicesearchsettings.h"
#include <QHostAddress>
#include "bindyusermodel.h"
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

QString ProtocolSelectionStyle = {
"border - radius: 2px;"
};


PageProgramConfigWgt::PageProgramConfigWgt(QWidget *parent, DeviceSearchSettings* _dss) :
    QWidget(parent),
    ui(new Ui::PageProgramConfigWgt)
{
    ui->setupUi(this);

	dss = _dss;
	try {
		bindy = new bindy::Bindy(BindyKeyfileName().toStdString(), false, false);
	} catch (std::runtime_error &e) {
		(void)e;
	}
	conn_id = bindy::conn_id_invalid;

	x_icon.addFile(":/settingsdlg/images/settingsdlg/x.png");
	w_icon.addFile(":/settingsdlg/images/settingsdlg/wrench.png");
	connect( ui->detectBtn, SIGNAL( clicked() ), this, SLOT( DetectHosts() ) );
	connect( ui->tableWidget, SIGNAL( cellClicked (int, int) ), this, SLOT( slotCellClicked( int, int ) ) );
	connect( ui->tableWidget, SIGNAL( cellChanged (int, int) ), this, SLOT( slotCellChanged( int, int ) ) );

	connect(ui->pushButton_toRight, SIGNAL(clicked()), this, SLOT(copyUsersLocalToRemote()));
	connect(ui->pushButton_toLeft, SIGNAL(clicked()), this, SLOT(copyUsersRemoteToLocal()));
	connect(ui->pushButton_trashLocal, SIGNAL(clicked()), this, SLOT(deleteUsersLocal()));
	connect(ui->pushButton_trashRemote, SIGNAL(clicked()), this, SLOT(deleteUsersRemote()));
	connect(ui->pushButton_newLocal, SIGNAL(clicked()), this, SLOT(createUserLocal()));
	connect(ui->pushButton_newRemote, SIGNAL(clicked()), this, SLOT(createUserRemote()));
	fixGrayout(ui->networkChk);

	bindy::user_vector_t empty;
	ui->localUserTableView->setModel(new BindyUserModel(empty));
	ui->remoteUserTableView->setModel(new BindyUserModel(empty));

	ui->localUserTableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	ui->remoteUserTableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	srand(time(0));

	// No servers found by default
	this->displayStatusServerCount(0);
}

PageProgramConfigWgt::~PageProgramConfigWgt()
{
    delete ui;
	delete bindy;
}

void PageProgramConfigWgt::DetectHosts()
{
	QPushButton *btn = ui->detectBtn;
	QString btn_text = btn->text();
	ui->detectBtn->setText("Detecting...");
	ui->detectBtn->setEnabled(false);

	QApplication::processEvents();

	libximc::device_enumeration_t tmp_enum;
	libximc::device_network_information_t netinfo;

	libximc::set_bindy_key(BindyKeyfileName().toLocal8Bit());
	tmp_enum = libximc::enumerate_devices(ENUMERATE_PROBE | ENUMERATE_NETWORK, "addr=");
	if (tmp_enum == 0) {
		displayStatus("Failure in enumerate_devices.");
		return;
	}
	int count = libximc::get_device_count(tmp_enum);
	std::vector<std::pair<QString, QString>> v;

	// get information about servers from each controller network enumerate info
	for (int i=0; i<count; i++) {
		result_t result = libximc::get_enumerate_device_network_information(tmp_enum, i, &netinfo);
		if (result == result_ok && netinfo.ipv4 != 0) {  // ignore all-zero address coming from empty memory of virtual controller
			v.push_back(std::make_pair(QString("xi-net://"), QHostAddress(ntohl(netinfo.ipv4)).toString()));
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
		QComboBox *boxProtocol;
		boxProtocol = qobject_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 0));
		v.push_back( std::make_pair(boxProtocol->currentText(), ui->tableWidget->item(i, 1)->text()) );
	}
	// once more sort and clear dupes
	std::sort(v.begin(), v.end()); 
    v.erase(std::unique(v.begin(), v.end()), v.end());

	QList<std::pair<QString, QString>> list = QList<std::pair<QString, QString>>::fromVector(QVector<std::pair<QString, QString>>::fromStdVector(v));

	SetTable(list);

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

	dss->scheme_host_pairs.clear();
	
	for (int i = 0; i < ui->tableWidget->rowCount()-1; ++i) { // last row is edit row which has no items
		QComboBox *boxProtocol;
		boxProtocol = qobject_cast<QComboBox*>(ui->tableWidget->cellWidget(i, 0));
		dss->scheme_host_pairs.append(std::make_pair(boxProtocol->currentText(), ui->tableWidget->item(i, 1)->text()));
	}
}

void PageProgramConfigWgt::FromClassToUi()
{
	ui->ProbeDevBox->setChecked(dss->Enumerate_probe);
	ui->NonXimcBox->setChecked(dss->Enumerate_all_com);
	ui->networkChk->setChecked(dss->Enumerate_network);
	ui->virtualDevicesSpinBox->setValue(dss->Virtual_devices);

	SetTable(dss->scheme_host_pairs);
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

void PageProgramConfigWgt::slotSchemeChanged()
{
	this->FromUiToSettings();
}

void PageProgramConfigWgt::displayStatus (QString text)
{
	ui->infoLabel->setText(text);
}

void PageProgramConfigWgt::displayStatusServerCount(uint64_t count)
{
	displayStatus(QString("%1 server(s) found.").arg(count));
}

void PageProgramConfigWgt::refreshUserLists()
{
	try {
		auto users_future = bindy->list_users_remote(conn_id);
		auto status = users_future.wait_for(std::chrono::milliseconds(FUTURE_WAIT_MS));
		if (status != std::future_status::ready)
			throw std::runtime_error("timed out waiting for result");
		remote_users = users_future.get();

		local_users = bindy->list_users_local();
	}
	catch (std::runtime_error &e) {
		displayStatus("Exception: " + QString(e.what()));
		return;
	}
	catch (...) {
		return;
	}
	delete ui->localUserTableView->model();
	delete ui->remoteUserTableView->model();

	ui->localUserTableView->setModel(new BindyUserModel(local_users));
	ui->remoteUserTableView->setModel(new BindyUserModel(remote_users));
}

void PageProgramConfigWgt::slotCellClicked ( int row, int column )
{
	if (column == 2 && row != ui->tableWidget->rowCount()-1)
		ui->tableWidget->removeRow(row);

	if (column == 3 && row != ui->tableWidget->rowCount()-1) {
		// First clear the old models so that lists become greyed out
		delete ui->localUserTableView->model();
		delete ui->remoteUserTableView->model();

		// Reconnect to specified address
		std::string address = ui->tableWidget->item(row, 1)->text().toStdString();
		bindy->disconnect(conn_id);
		try {
			conn_id = bindy->connect(address, std::string());
		}
		catch (...) { // CryptoPP::Socket::Err
			conn_id = bindy::conn_id_invalid;
		}
		if (conn_id == bindy::conn_id_invalid) {
			displayStatus("Connection failed");
			ui->userManageBox->setEnabled(false);
			return;
		}
		ui->userManageBox->setEnabled(true);
		displayStatus("Connection ok");

		refreshUserLists();
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

		//по горизонтали растянем, по вертикали - как решит программа :)
		boxProtocol->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		boxProtocol->addItem(XI_NET);
		boxProtocol->addItem(XI_UDP);
		boxProtocol->addItem(XI_TCP);
		
		//вставляем в таблицу QTableWidget в колонку №0
		ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 0, boxProtocol);
		QObject::connect(boxProtocol, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSchemeChanged()));
	} else if (column == 1 && row != tw->rowCount()-1 && tw->item(row, column)->text().trimmed() == QString("") ) { // remove empty line
		tw->removeRow(row);
	}
	tw->blockSignals(false);

	// Update list of IPs and schemes
	FromUiToSettings();
}

void PageProgramConfigWgt::SetTable(QList<std::pair<QString, QString>> scheme_host_pairs)
{
	QTableWidget *tw = ui->tableWidget;

	tw->blockSignals(true);

	tw->clearContents();
	tw->setRowCount(scheme_host_pairs.size()+1);
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

	for (int i=0; i < tw->rowCount()-1 ; ++i) {
		tw->setItem(i, 0, new QTableWidgetItem( scheme_host_pairs.at(i).first ) );
		tw->setItem(i, 1, new QTableWidgetItem( scheme_host_pairs.at(i).second ) );
		QComboBox *boxProtocol = new QComboBox;
		boxProtocol->setStyleSheet(ProtocolSelectionStyle);
		int curr_ind;
		//по горизонтали растянем, по вертикали - как решит программа :)
		boxProtocol->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		boxProtocol->addItem(XI_NET);
		boxProtocol->addItem(XI_UDP);
		boxProtocol->addItem(XI_TCP);
		if (scheme_host_pairs.at(i).first == XI_UDP)
			curr_ind = 1;
		else if (scheme_host_pairs.at(i).first == XI_TCP)
			curr_ind = 2;
		else
			curr_ind = 0;

		boxProtocol->setCurrentIndex(curr_ind);
		//вставляем в таблицу QTableWidget в колонку №0
		ui->tableWidget->setCellWidget(i, 0, boxProtocol);

		QTableWidgetItem* icon_item = new QTableWidgetItem();
		icon_item->setIcon(x_icon);
		icon_item->setFlags(Qt::ItemIsEnabled);
		tw->setItem(i, 2, icon_item );

		QTableWidgetItem* icon_item2 = new QTableWidgetItem();
		icon_item2->setIcon(w_icon);
		icon_item2->setFlags(Qt::ItemIsEnabled);
		tw->setItem(i, 3, icon_item2 );

		QObject::connect(boxProtocol, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSchemeChanged()));
	}

	QComboBox *boxProtocol = new QComboBox;
	boxProtocol->setStyleSheet(ProtocolSelectionStyle);

	//по горизонтали растянем, по вертикали - как решит программа :)
	boxProtocol->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	boxProtocol->addItem(XI_NET);
	boxProtocol->addItem(XI_UDP);
	boxProtocol->addItem(XI_TCP);
	ui->tableWidget->setCellWidget(scheme_host_pairs.size(), 0, boxProtocol);

	// this is the edit line
	tw->setItem(scheme_host_pairs.size(), 2, new QTableWidgetItem());
	tw->setItem(scheme_host_pairs.size(), 3, new QTableWidgetItem());
	tw->item(scheme_host_pairs.size(), 2)->setFlags(Qt::NoItemFlags);
	tw->item(scheme_host_pairs.size(), 3)->setFlags(Qt::NoItemFlags);

	tw->blockSignals(false);
}

void PageProgramConfigWgt::copyUsersLocalToRemote ( )
{
	bindy::user_vector_t copy_list;
	QItemSelectionModel *local_model = ui->localUserTableView->selectionModel();
	if (local_model == nullptr) {
		return;
	}
	for (auto item : local_model->selectedRows()) {
		copy_list.push_back( local_users.at( item.row() ) );
	}
	for (auto user : copy_list) {
		try {
			bindy->add_user_remote(conn_id, user.name, user.key);
		} catch (...) {
			displayStatus(QString("Failed to copy user %1 local -> remote").arg(QString::fromStdString(user.name)));
		}
	} 
	refreshUserLists();
}

void PageProgramConfigWgt::copyUsersRemoteToLocal ( )
{
	bindy::user_vector_t copy_list;
	QItemSelectionModel *remote_model = ui->remoteUserTableView->selectionModel();
	if (remote_model == nullptr) {
		return;
	}
	for (auto item : remote_model->selectedRows()) {
		copy_list.push_back( remote_users.at( item.row() ) );
	}
	for (auto user : copy_list) {
		try {
			bindy->add_user_local(user.name, user.key);
		} catch (...) {
			displayStatus(QString("Failed to copy user %1 remote -> local").arg(QString::fromStdString(user.name)));
		}
	}
	refreshUserLists();
}

void PageProgramConfigWgt::deleteUsersLocal ( )
{
	bindy::user_vector_t del_list;
	QItemSelectionModel *local_model = ui->localUserTableView->selectionModel();
	if (local_model == nullptr) {
		return;
	}
	for (auto item : local_model->selectedRows()) {
		del_list.push_back( local_users.at( item.row() ) );
	}
	try {
		for (auto user : del_list) {
			bindy->del_user_local(user.uid);
		}
	} catch (...) {
		displayStatus("Failed to delete local users");
	}
	refreshUserLists();
}

void PageProgramConfigWgt::deleteUsersRemote ( )
{
	bindy::user_vector_t del_list;
	QItemSelectionModel *remote_model = ui->remoteUserTableView->selectionModel();
	if (remote_model == nullptr) {
		return;
	}
	for (auto item : remote_model->selectedRows()) {
		del_list.push_back( remote_users.at( item.row() ) );
	}
	try {
		for (auto user : del_list) {
			bindy->del_user_remote(conn_id, user.uid);
		}
	} catch (...) {
		displayStatus("Failed to delete remote users");
	}
	refreshUserLists();
}

typedef struct {
	bool ok;
	std::string name;
	bindy::aes_key_t key;
} namepair_t;

namepair_t askForNameKey()
{
	namepair_t result;
	QMessageBox msgBox;
	msgBox.setWindowTitle("Enter user name");
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	QAbstractButton *okBtn = msgBox.button(QMessageBox::Ok);
	QLineEdit *line = new QLineEdit();
	msgBox.layout()->addWidget(line);
	msgBox.exec();
	msgBox.layout()->removeWidget(line);
	result.name = line->text().toStdString();
	int keylen = sizeof(result.key);
	for (int i = 0 ; i < keylen ; i++) {
		result.key.bytes[i] = static_cast<uint8_t>(rand());
	}
	result.ok = (msgBox.clickedButton() == okBtn);
	delete line;

	return result;
}

void PageProgramConfigWgt::createUserLocal()
{
	auto result = askForNameKey();
	if (result.ok) {
		try {
			bindy->add_user_local(result.name, result.key);
		}
		catch (...) {
			displayStatus("Failed to create local user");
		}
		refreshUserLists();
	}
}

void PageProgramConfigWgt::createUserRemote()
{
	auto result = askForNameKey();
	if (result.ok) {
		try {
			bindy->add_user_remote(conn_id, result.name, result.key);
		}
		catch (...) {
			displayStatus("Failed to create remote user");
		}
		refreshUserLists();
	}
}
