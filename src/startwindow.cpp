#include <QDesktopWidget>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDate>
#include <qevent.h>
#include <ui_startwindow.h>
#include <startwindow.h>
#include <functions.h>
#include <main.h>
#include <xsettings.h>
#include <QMutex>
#include <nodevicehelperdlg.h>
#include <qtablewidget.h>

//скорости анимированной иконки
#define SLOW_SPEED	15
#define FAST_SPEED	200

const char* str_open = "Settings >>";
const char* str_close = "Settings <<";

extern bool first_start;
StartWindow::StartWindow(QWidget *parent): QWidget(parent), m_ui(new Ui::StartWindowClass)
{
	m_ui->setupUi(this);

	setWindowFlags(Qt::Window);

	m_ui->titleLbl->setText(QString("mDrive Direct Control ")+QString(XILAB_VERSION));
	m_ui->releaseDateLbl->setText(QString("release date: ")+compileDate());
	m_ui->exBtn->setText(str_open);

	SetPicture();

	movie.setFileName(":/startwindow/images/startwindow/arrows.gif");
	m_ui->pixLbl->setMovie(&movie);
	movie.setSpeed(SLOW_SPEED);
	movie.setCacheMode(QMovie::CacheAll);
	movie.start();
	
	//m_ui->buttonsFrame->setVisible(false);
    setVisibleFrameButtons(false);
	move(qApp->desktop()->availableGeometry(this).center()-rect().center());
	b_move = false;
	
	// set header for devices table
	this->m_ui->deviceListTable->setRowCount(0);
	this->m_ui->deviceListTable->setColumnCount(3);
	QStringList header;
	header << QString("URI") << QString("S/N") << QString("Friendly name");
	this->m_ui->deviceListTable->setHorizontalHeaderLabels(header);

	// set row height
	QHeaderView *h_view = this->m_ui->deviceListTable->verticalHeader();
	h_view->setResizeMode(QHeaderView::Fixed);
	h_view->setDefaultSectionSize(20);

	// set auto-size last column
	this->m_ui->deviceListTable->horizontalHeader()->setStretchLastSection(true);

	// disallow edit
	this->m_ui->deviceListTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	// select only rows ()
	this->m_ui->deviceListTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	// enable sorting
	this->m_ui->deviceListTable->setSortingEnabled(true);

	// load cells widths
	this->loadTableSettings(MakeProgramConfigFilename());
	

	targetSpeed = FAST_SPEED;
	lastFrame = 0;
	timer.start(10);

#ifndef __LINUX__
	//m_ui->linuxFrame->hide();
#endif
	m_ui->settings_frame->hide();

	dss = new DeviceSearchSettings ( MakeProgramConfigFilename() );
	
	pagepc = new PageProgramConfigWgt(NULL, dss);
	pagepc->FromClassToUi();
	((QGridLayout*)m_ui->settings_frame->layout())->addWidget(pagepc);

	devicethread = new DeviceThread(this, new DeviceInterface(), dss);

	qRegisterMetaType<QList<Qt::ItemFlags> >("QList<Qt::ItemFlags>");
	qRegisterMetaType<QList<uint32_t> >("QList<uint32_t>");

	connect(devicethread, SIGNAL(finished(bool, QStringList, QStringList, QStringList, QList<uint32_t>, QList<Qt::ItemFlags>)), this, SLOT(deviceListRecieved(bool, QStringList, QStringList, QStringList, QList<uint32_t>, QList<Qt::ItemFlags>)));

	/*
	 * When pressed, a timer starts to control double-tapping so that the hint is not displayed.
	*/
	connect(m_ui->deviceListTable, SIGNAL(itemPressed(QTableWidgetItem*)), this, SLOT(itemPressed(QTableWidgetItem*)));
	connect(m_ui->deviceListTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(itemDoubleClicked(QTableWidgetItem*)));
	connect(m_ui->deviceListTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(itemClicked(QTableWidgetItem*)));
	connect(m_ui->deviceListTable, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(SelClic()));

	connect(m_ui->selectBtn, SIGNAL(clicked()), this, SLOT(selectBtnClicked()));
	connect(m_ui->retryBtn, SIGNAL(clicked()), this, SLOT(retryBtnClicked()));
	connect(m_ui->exBtn, SIGNAL(clicked()), this, SLOT(exBtnClicked()));
	connect(m_ui->noDevicesLinuxHelper, SIGNAL(clicked()), this, SLOT(noDevicesLinuxHelperClicked()));
	connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()), Qt::DirectConnection);
	/*
	 * Timers for managing the opening mode hint
	 * timer1 - double-click control.
	 * timer2 - time when the hint is displayed.
	*/
	connect(&timer1, SIGNAL(timeout()), this, SLOT(timer1Full()), Qt::DirectConnection);
	connect(&timer2, SIGNAL(timeout()), this, SLOT(timer2Full()), Qt::DirectConnection);

	this->loadWindowGeometry(MakeProgramConfigFilename());
	inited = true;
}

StartWindow::~StartWindow()
{
	inited = false;
	delete m_ui;
}

void StartWindow::saveTableSettings(QString filename)
{
	XSettings settings(filename, QSettings::IniFormat, QIODevice::ReadWrite);

	settings.beginGroup("Device_table");

	QByteArray table_state = this->m_ui->deviceListTable->horizontalHeader()->saveState();

	settings.setValue("State", table_state);
	settings.endGroup();
}

void StartWindow::loadTableSettings(QString filename)
{
	XSettings settings(filename, QSettings::IniFormat, QIODevice::ReadOnly);

	settings.beginGroup("Device_table");

	QByteArray table_state = settings.value("State", QByteArray()).toByteArray();

	if (table_state.size() == 0)
	{
		/*
		 * No table settings, set default widths
		 */
		int width = this->m_ui->deviceListTable->columnWidth(Columns::COLUMN_URI) * 2;  // #19192-23: stretch uri column
		this->m_ui->deviceListTable->setColumnWidth(Columns::COLUMN_URI, width);
	}

	this->m_ui->deviceListTable->horizontalHeader()->restoreState(table_state);

	settings.endGroup();
}


void StartWindow::loadWindowGeometry(QString filename)
{
	XSettings settings(filename, QSettings::IniFormat, QIODevice::ReadOnly);

	settings.beginGroup("Start_window");

	QByteArray geometry = settings.value("Geometry", QByteArray()).toByteArray();

	qDebug() << "Geometry loaded: " << geometry;

	if (geometry.size() != 0)
	{
		qDebug() << "Restore";
		this->restoreGeometry(geometry);
	}

	settings.endGroup();
}

void StartWindow::saveWindowGeometry(QString filename)
{
	XSettings settings(filename, QSettings::IniFormat, QIODevice::ReadWrite);

	settings.beginGroup("Start_window");

	QByteArray geometry = this->saveGeometry();

	qDebug() << "Save geometry " << geometry;

	settings.setValue("Geometry", geometry);

	settings.endGroup();
}

void StartWindow::SetPicture()
{
	QDate today = QDate::currentDate();

	if(today.daysInYear() - today.dayOfYear() <= 7 || today.dayOfYear() < 7)	//новогодняя картинка за неделю до НГ и в течении недели после
	{
		m_ui->motorLbl->setPixmap(QPixmap(":/startwindow/images/startwindow/new_year.png"));
	}
	else			//стандартная картинка
	{
		m_ui->motorLbl->setPixmap(QPixmap(":/startwindow/images/startwindow/motor_icon.png"));
	}
}

void  StartWindow::ShowDeviseSelClic()
{
	QModelIndexList list = m_ui->deviceListTable->selectionModel()->selectedRows();
	QModelIndexList::iterator i;
	int count_row = 0;

	for (i = list.begin(); i < list.end(); ++i)
	{
		count_row++;
	}

	if (count_row == 0)
	{
		QToolTip::showText(QPoint(cursor().pos().x(), cursor().pos().y() + 20), "No axes are selected.");
		m_ui->axisSelect->setText("No axes are selected.");

	}
	if (count_row == 1)
	{
		QToolTip::showText(QPoint(cursor().pos().x(), cursor().pos().y() + 20), "The app will run in single-axis mode.");
		m_ui->axisSelect->setText("One axis is selected.");
	}
	else
	{
		QToolTip::showText(QPoint(cursor().pos().x(), cursor().pos().y() + 20), "The app will run in multi-axis mode.");
		if (count_row == 2)
			m_ui->axisSelect->setText("Two axes are selected.");
		else
			if (count_row == 3)
				m_ui->axisSelect->setText("three axes are selected.");
			else
				m_ui->axisSelect->setText(QString::number(count_row) + " axes are selected.");
	}

	timer2.start(TIME_SHOW);
	
}

void StartWindow::startSearching()
{
	state = STATE_THINKING;
	targetSpeed = FAST_SPEED;
	m_ui->infoLbl->setText("Searching devices...");
	{
		QMutexLocker locker(&mutex);
		selectedDevices.clear();
	}
	devicethread->start();
}

void StartWindow::setVisibleFrameButtons(bool visible)
{
    m_ui->retryBtn->setVisible(visible);
    m_ui->selectBtn->setVisible(visible);
}

void StartWindow::deviceListRecieved(bool enum_ok, QStringList names, QStringList descriptions, QStringList friendlyNames, QList<uint32_t> serials, QList<Qt::ItemFlags> flags)
{
	Q_UNUSED(descriptions)
	Q_UNUSED(flags)
	this->m_ui->deviceListTable->setSortingEnabled(false);

	if (enum_ok) {
		m_ui->infoLbl->setText(QString("Searching devices... %1 found.").arg(names.size()));
	} else {
		m_ui->infoLbl->setText(QString("Searching devices... enumerate failed."));
	}
	targetSpeed = SLOW_SPEED;
	timer.start();
	//найдено 0 девайсов и 0 виртуальных

	m_ui->noDevicesLinuxHelper->setVisible(names.empty());
	//m_ui->buttonsFrame->setVisible(true);
    setVisibleFrameButtons(true);
	m_ui->deviceListTable->clearContents();
	m_ui->deviceListTable->setRowCount(0);

	if ((names.size() == 0) && (dss->Virtual_devices == 0)) {
		state = STATE_QUESTION;
		m_ui->selectBtn->setEnabled(false);

		targetSpeed = SLOW_SPEED;
		m_ui->retryBtn->setFocus(Qt::ActiveWindowFocusReason);
	}
	else { //найдено больше 1 девайса или мы наблюдали ошибку инициализации
		state = STATE_LIST;
		//m_ui->deviceListTable->setVisible(true);
        setVisibleFrameButtons(true);
		m_ui->selectBtn->setEnabled(true);
		for(int i=0; i<names.size(); i++){
			m_ui->deviceListTable->insertRow(m_ui->deviceListTable->rowCount());
			m_ui->deviceListTable->setItem(i, Columns::COLUMN_URI, new QTableWidgetItem(names[i]));
			m_ui->deviceListTable->setItem(i, Columns::COLUMN_SERIAL, new QTableWidgetItem(QString::number(serials.at(i))));
			m_ui->deviceListTable->setItem(i, Columns::COLUMN_FRIENDLY_NAME, new QTableWidgetItem(friendlyNames.at(i)));
		}

		for (unsigned int i = 0; i<dss->Virtual_devices; i++){

			QString url = QString("xi-emu:///%1?serial=%2").arg(VirtualControllerBlobFilename(i + 1)).arg(i + 1);
			QString serial = QString::number(i + 1);

			m_ui->deviceListTable->insertRow(m_ui->deviceListTable->rowCount());
			m_ui->deviceListTable->setItem(m_ui->deviceListTable->rowCount() - 1, Columns::COLUMN_URI, new QTableWidgetItem(url));
			m_ui->deviceListTable->setItem(m_ui->deviceListTable->rowCount() - 1, Columns::COLUMN_SERIAL, new QTableWidgetItem(serial));
			m_ui->deviceListTable->setItem(m_ui->deviceListTable->rowCount() - 1, Columns::COLUMN_FRIENDLY_NAME, new QTableWidgetItem(""));

		}
		m_ui->deviceListTable->setCurrentCell(0, 1);
		m_ui->selectBtn->setEnabled(m_ui->deviceListTable->rowCount() > 1);		
	}

	LoadAxisConfig(serials);

	this->m_ui->deviceListTable->setSortingEnabled(true);
}

void StartWindow::itemPressed(QTableWidgetItem *item)
{
	Q_UNUSED(item)
	timer1.start(TIME_DOUBLE_CLICK);
}

void StartWindow::itemClicked(QTableWidgetItem *item)
{
	Q_UNUSED(item)
	m_ui->selectBtn->setEnabled(true);
}

void StartWindow::itemDoubleClicked(QTableWidgetItem  *item)
{
	QMutexLocker locker(&mutex);
	selectedDevices.clear();
	QString url = this->m_ui->deviceListTable->item(item->row(), Columns::COLUMN_URI)->text();

	selectedDevices.push_back(url);

	timer1.stop();
}

void StartWindow::timerUpdate()
{
	int currentSpeed = movie.speed();
	if((movie.state() == QMovie::NotRunning) && (targetSpeed > currentSpeed)) {
		movie.start();
		movie.jumpToFrame(lastFrame);
		m_ui->pixLbl->setMovie(&movie);
		m_ui->pixLbl->update();
		return;
	}
	if((movie.state() == QMovie::Running) && 
	   (targetSpeed <= currentSpeed) && 
	   (currentSpeed < (SLOW_SPEED + 5))){ 
			movie.stop();
			timer.stop();
			movie.setSpeed(targetSpeed);
			lastFrame = movie.currentFrameNumber();
			if(state == STATE_QUESTION){
				m_ui->pixLbl->setPixmap(QPixmap(":/startwindow/images/startwindow/question.png"));
				movie.setCacheMode(QMovie::CacheAll);
			}
			else if(state == STATE_LIST){
				m_ui->pixLbl->setPixmap(QPixmap(":/startwindow/images/startwindow/list.png"));
				movie.setCacheMode(QMovie::CacheAll);
			}
			return;
	}

	if(movie.state() == QMovie::Running){
		if(currentSpeed < targetSpeed){
			movie.setSpeed(currentSpeed+2);
			if(currentSpeed+2 > targetSpeed)
				movie.setSpeed(targetSpeed);
		}
		else if(currentSpeed > targetSpeed){
			movie.setSpeed(currentSpeed-2);
			if(currentSpeed-2 < targetSpeed)
				movie.setSpeed(targetSpeed);	
		}
	}
	
	m_ui->pixLbl->update();
}

void StartWindow::timer1Full()
{
	ShowDeviseSelClic();
	timer1.stop();
}

void StartWindow::timer2Full()
{
	QToolTip::hideText();
	timer2.stop();
}

void StartWindow::SelClic()
{
	timer1.start(TIME_DOUBLE_CLICK);
}

void StartWindow::selectBtnClicked()
{
	try {
		XSettings settings(MakeProgramConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);

		QString sn_str;
		QList<uint32_t> sorted_serials;

		QModelIndexList list = m_ui->deviceListTable->selectionModel()->selectedRows();
		QModelIndexList::iterator i;

		for (i = list.begin(); i < list.end(); ++i)
		{
			QString serial = m_ui->deviceListTable->item((*i).row(), Columns::COLUMN_SERIAL)->text();
			sorted_serials.append(serial.toUInt());
		}

		qSort(sorted_serials);
		sn_str.append("SN_");

		for (int i=0; i<sorted_serials.length(); i++){
			sn_str.append( toStr(sorted_serials.at(i)) + QString("-") );
			if (sorted_serials.first() == 0 && sorted_serials.last() == 0) // skip check for same serials if all are zero (PROBE flag unset)
				continue;
			if (sorted_serials.count(sorted_serials.at(i)) != 1) { // check for same serials
				this->hide();
				throw my_exception("Devices have same serials");
			}
		}
		sn_str.chop(1);
		settings.beginGroup(sn_str);
		QStringList keys = settings.childKeys();
		serials.clear();
		for (int i=0; i<keys.length(); i++) {
			unsigned int sn = settings.value(keys.at(i)).toUInt();
			serials.append(sn);
		}
		settings.endGroup();
      
		QMutexLocker locker(&mutex);
		selectedDevices.clear();
		for (i = list.begin(); i != list.end(); ++i){
			QString url = m_ui->deviceListTable->item((*i).row(), Columns::COLUMN_URI)->text();
			selectedDevices.push_back(url);
		}

	}
	catch(my_exception &e){
		QMessageBox::warning(0, "An exception has occurred", e.text());
	}
}

void StartWindow::retryBtnClicked()
{
	pagepc->FromUiToClass();
	targetSpeed = FAST_SPEED;
	movie.setSpeed(FAST_SPEED);
	movie.start();
	m_ui->pixLbl->setMovie(&movie);
	m_ui->pixLbl->update();
	//m_ui->buttonsFrame->setVisible(false);
    setVisibleFrameButtons(false);
	startSearching();
}

void StartWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
		lastPoint = event->pos();      
        b_move = true;
    }
}

void StartWindow::mouseMoveEvent(QMouseEvent *event)
{
	if(b_move){
        move(event->globalX()-lastPoint.x(),
             event->globalY()-lastPoint.y());
	}
}

void StartWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
            b_move = false;
        }
}

QList<QString> StartWindow::getSelectedDevices()
{
	QMutexLocker locker(&mutex);
	return selectedDevices;
}

void StartWindow::keyPressEvent(QKeyEvent *event)
{
	if((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::AltModifier) && (event->modifiers() & Qt::ShiftModifier)){
		m_ui->titleLbl->setText("Happy New Year!!!");
	}
}

void StartWindow::hideEvent(QHideEvent * event)
{
	Q_UNUSED(event)
	this->saveTableSettings(MakeProgramConfigFilename());

	/*
	 * Hide right additional window before geometry saving
	 */
	if (m_ui->settings_frame->isVisible()) {
		this->hideRight();
	}

	this->saveWindowGeometry(MakeProgramConfigFilename());

	pagepc->FromUiToSettings();
}


void StartWindow::LoadAxisConfig(QList<uint32_t> all_serials)
{
	XSettings settings(MakeProgramConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
	QString sn_str;

	settings.beginGroup("Multiaxis");
	sn_str = settings.value("last_group").toString();
	settings.endGroup();

	settings.beginGroup(sn_str);
	QStringList keys = settings.childKeys();
	bool all_devices_found = (all_serials.count() > 0), group_not_empty = false;
	serials.clear();
	for (int i=0; i<keys.length(); i++) {
		bool ok;
		keys.at(i).toUInt(&ok);
		if (!ok)
			continue;

		group_not_empty = true;
		unsigned int sn = settings.value(keys.at(i)).toUInt();
		serials.append(sn);
		if (!all_serials.contains( sn )) {
			all_devices_found = false;
			break;
		}
	}
	settings.endGroup();
}

void StartWindow::hideRight()
{
	QWidget * right = m_ui->settings_frame;

	right->hide();

	m_ui->exBtn->setText(str_open);

	this->resize(this->size().width() - right->sizeHint().width(), this->size().height());
}

void StartWindow::showRight()
{
	QWidget * right = m_ui->settings_frame;

	/*
	* Increase window size by the width of the settings frame.
	* This width is fixed due to the settings frame size policy
	*/
	this->resize(this->size().width() + right->sizeHint().width(), this->size().height());
	right->show();

	m_ui->exBtn->setText(str_close);
}

void StartWindow::closeEvent(QCloseEvent *event)
{
	QMutexLocker locker(&mutex);
	hide();
	selectedDevices.insert(0, "nodevices");
}

void StartWindow::exBtnClicked()
{
	QWidget * right = m_ui->settings_frame;

	if ( right->isVisible() ) {
		this->hideRight();
	} else {
		this->showRight();
	}
}

bool StartWindow::return_probe() 
{
return dss->Enumerate_probe; 
}

void StartWindow::noDevicesLinuxHelperClicked()
{
#if defined(WIN32) || defined(WIN64)
	NoDeviceHelperDlg dlg(this, "No devices found? Check these settings.");
	dlg.enableWin();
	dlg.exec();
#endif	

#ifdef __LINUX__
	NoDeviceHelperDlg dlg(this, "No devices found on Linux? Check these settings.");
	dlg.enableLinux();
	dlg.exec();
#endif

#ifdef __APPLE__
	NoDeviceHelperDlg dlg(this, "No devices found? Check these settings.");
	dlg.enableApple();
	dlg.exec();
#endif
}
