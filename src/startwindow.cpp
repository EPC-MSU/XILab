#include <QDesktopWidget>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDate>
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

// список этапов помощника
enum HelpStep
{
	OpenDevaseStep = 1,
	SetupDevaseStep,
	InitialSettingsStep,
	LoadSettingsStep,
	ProfileUploadStep,
	ApplySettingsStep,
	MotionStep,
	MotionSettingsStep,
	UserUnitStep
};

// число шагов помощника
int countStep = 2;
int startStep = 1;

// массив названий изображений
QString PixmapString[10] = {
	"",
	":/mainwindow/images/mainwindow/step_opendevase.png",
	":/mainwindow/images/mainwindow/step_setupdevase.png",
	":/mainwindow/images/mainwindow/step_1.png",
	":/mainwindow/images/mainwindow/step_2.png",
	":/mainwindow/images/mainwindow/step_3.png",
	":/mainwindow/images/mainwindow/step_4.png",
	":/mainwindow/images/mainwindow/step_5.png",
	":/mainwindow/images/mainwindow/step_6.png",
	":/mainwindow/images/mainwindow/step_7.png"
};

// массив сообщений
QString TextString[10] = {
	"",
	"Step open devase: you can select any device from the last by double-clicking on the corresponding line. To select multiple devices, use the Ctrl and Shift buttons.Opening multiple devices is performed by the Open selected button, as shown below.",
	"Step setup devase: it is important when connecting a new positioner to install the correct profile for it in order to avoid breakage. To do this, click the Settings button - as shown in the figure below.",
	"Step 1: it is important when connecting a new positioner to install the correct profile for it in order to avoid breakage. To do this, click the Settings button - as shown in the figure below.",
	"Step 2: in the settings window that appears, click Load settings from file...  - as shown in the figure below. ",
	"Step 3: in the window that opens, select the folder of the corresponding manufacturer and the profile corresponding to the positioner, then click Open",
	"Step 4: to apply this profile, click Apply or Ok in the Settings window. To not use the selected profile, click Cancel.",
	"Step 5: motion control is carried out from the main Xlab window. Using the buttons on the slider, you can make movements in both directions or make a stop. Precise movements can be carried out by setting values in the fields for moving and shifting, followed by pressing the corresponding buttons.",
	"Step 6: to adjust motion parameters such as speed, acceleration, etc., you must use one of the three pages of the Settings window, depending on the type of motor used in the positioner, as shown below.",
	"Step 7: the XiLab application also allows you to make movement both in motor units, such as steps revolutions per minute, etc., and in user units. When using user units, it is necessary to check the box in the User unit tab of the Settings window, as shown below."
};


const char* str_open = "Settings >>";
const char* str_close = "Settings <<";

extern bool first_start;
StartWindow::StartWindow(QWidget *parent): QWidget(parent), m_ui(new Ui::StartWindowClass)
{
	m_ui->setupUi(this);

	setWindowFlags(Qt::Window |Qt::CustomizeWindowHint);

	m_ui->titleLbl->setText(QString("XILab ")+QString(XILAB_VERSION));
	m_ui->releaseDateLbl->setText(QString("release date: ")+compileDate());
	m_ui->exBtn->setText(str_open);

	SetPicture();

	movie.setFileName(":/startwindow/images/startwindow/arrows.gif");
	m_ui->pixLbl->setMovie(&movie);
	movie.setSpeed(SLOW_SPEED);
	movie.setCacheMode(QMovie::CacheAll);
	movie.start();
	
	m_ui->buttonsFrame->setVisible(false);
	move(qApp->desktop()->availableGeometry(this).center()-rect().center());
	b_move = false;
	
	// set header for devices table
	this->m_ui->deviceListTable->setRowCount(0);
	this->m_ui->deviceListTable->setColumnCount(4);
	QStringList header;
	header << QString("URI") << QString("Serial") << QString("Friendly name") << QString("Stage name");
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

	connect(devicethread, SIGNAL(finished(bool, QStringList, QStringList, QStringList, QStringList, QList<uint32_t>, QList<Qt::ItemFlags>)), this, SLOT(deviceListRecieved(bool, QStringList, QStringList, QStringList, QStringList, QList<uint32_t>, QList<Qt::ItemFlags>)));

	/*
	 * When pressed, a timer starts to control double-tapping so that the hint is not displayed.
	*/
	connect(m_ui->deviceListTable, SIGNAL(itemPressed(QTableWidgetItem*)), this, SLOT(itemPressed(QTableWidgetItem*)));
	connect(m_ui->deviceListTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(itemDoubleClicked(QTableWidgetItem*)));
	connect(m_ui->deviceListTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(itemClicked(QTableWidgetItem*)));
	connect(m_ui->deviceListTable, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(SelClic()));

	connect(m_ui->selectBtn, SIGNAL(clicked()), this, SLOT(selectBtnClicked()));
	connect(m_ui->retryBtn, SIGNAL(clicked()), this, SLOT(retryBtnClicked()));
	connect(m_ui->cancelBtn, SIGNAL(clicked()), this, SLOT(cancelBtnClicked()));
	connect(m_ui->openLastConfigBtn, SIGNAL(clicked()), this, SLOT(openLastConfigBtnClicked()));
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

	//showhelp();
}

StartWindow::~StartWindow()
{
	inited = false;
	//if (b)
	//	b->close();
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
			m_ui->axisSelect->setText("three axes are selected.");
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

void StartWindow::deviceListRecieved(bool enum_ok, QStringList names, QStringList descriptions, QStringList friendlyNames, QStringList positionerNames, QList<uint32_t> serials, QList<Qt::ItemFlags> flags)
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
	m_ui->buttonsFrame->setVisible(true);
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
		m_ui->deviceListTable->setVisible(true);
		m_ui->selectBtn->setEnabled(true);
		for(int i=0; i<names.size(); i++){
			m_ui->deviceListTable->insertRow(m_ui->deviceListTable->rowCount());
			m_ui->deviceListTable->setItem(i, Columns::COLUMN_URI, new QTableWidgetItem(names[i]));
			m_ui->deviceListTable->setItem(i, Columns::COLUMN_SERIAL, new QTableWidgetItem(QString::number(serials.at(i))));
			m_ui->deviceListTable->setItem(i, Columns::COLUMN_FRIENDLY_NAME, new QTableWidgetItem(friendlyNames.at(i)));
			m_ui->deviceListTable->setItem(i, Columns::COLUMN_STAGE, new QTableWidgetItem(positionerNames.at(i)));
		}

		for (unsigned int i = 0; i<dss->Virtual_devices; i++){

			QString url = QString("xi-emu:///%1?serial=%2").arg(VirtualControllerBlobFilename(i + 1)).arg(i + 1);
			QString serial = QString::number(i + 1);

			m_ui->deviceListTable->insertRow(m_ui->deviceListTable->rowCount());
			m_ui->deviceListTable->setItem(m_ui->deviceListTable->rowCount() - 1, Columns::COLUMN_URI, new QTableWidgetItem(url));
			m_ui->deviceListTable->setItem(m_ui->deviceListTable->rowCount() - 1, Columns::COLUMN_SERIAL, new QTableWidgetItem(serial));
			m_ui->deviceListTable->setItem(m_ui->deviceListTable->rowCount() - 1, Columns::COLUMN_FRIENDLY_NAME, new QTableWidgetItem(""));
			m_ui->deviceListTable->setItem(m_ui->deviceListTable->rowCount() - 1, Columns::COLUMN_STAGE, new QTableWidgetItem(""));

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
	m_ui->buttonsFrame->setVisible(false);
	startSearching();
}

void StartWindow::cancelBtnClicked()
{
	QMutexLocker locker(&mutex);
	hide();
	selectedDevices.insert(0, "nodevices");
}

void StartWindow::openLastConfigBtnClicked()
{
	QMutexLocker locker(&mutex);

	selectedDevices.clear();

	QSet<QString> set;
	QSet<QString>::iterator si;
	
	for (int i = 0; i < m_ui->deviceListTable->rowCount(); i++) {

		unsigned int sn = m_ui->deviceListTable->item(i, Columns::COLUMN_SERIAL)->text().toUInt();
		QString url = m_ui->deviceListTable->item(i, Columns::COLUMN_URI)->text();
		if (serials.contains( sn )) {
			set.insert( url );
		}
	}
	for (si = set.begin(); si != set.end(); ++si) {
		selectedDevices.append( *si );
	}
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

	m_ui->openLastConfigBtn->setEnabled(group_not_empty && all_devices_found);
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

void StartWindow::showhelp()
{
	XSettings settings(MakeProgramConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);

	settings.beginGroup("Start_window");
	int firstLaunch = 0;
	firstLaunch = settings.value("first_launch").toInt();
	settings.endGroup();
	if (firstLaunch == 0)
	{
		settings.beginGroup("Start_window");
		settings.setValue("first_launch", 1);
		settings.endGroup();

		b = new QWidget();
		b->setWindowTitle("Starting hints");
		b->move(this->x() - 440, this->y());
		b->setFixedWidth(420);
		b->setFixedHeight(600);

		QPixmap logo = QPixmap(PixmapString[1]);
		llogo = new QLabel(b);
		llogo->setGeometry(QRect(QPoint(10, 130), logo.size()));
		llogo->setPixmap(logo);
		lictext = new QLabel(b);
		QFont f("Courrier", 8);
		lictext->setFont(f);
		lictext->setGeometry(QRect(10,10, b->width() - 20, 110));
		lictext->setLineWidth(0);

		lictext->setText(TextString[1]);
		lictext->setWordWrap(true);
		QPushButton * but1 = new QPushButton(b);
		QPushButton * but2 = new QPushButton(b);
		but1->setFixedWidth(120);
		but1->setFixedHeight(50);
		but1->setGeometry(QRect(10, b->height() - 60, 150, b->height() - 10));
		but1->setFont(f);
		but1->setText("Previous step");

		but2->setFixedWidth(120);
		but2->setFixedHeight(50);
		but2->setGeometry(QRect(b->width() - 130, b->height() - 60, b->width() - 10, b->height() - 10));
		but2->setFont(f);
		but2->setText("Next step");
		currentStep = 1;

		QObject::connect(but1, SIGNAL(clicked()), this, SLOT(Onbut1Clicked()));
		QObject::connect(but2, SIGNAL(clicked()), this, SLOT(Onbut2Clicked()));

		b->show();
	}
}

void StartWindow::Onbut1Clicked()
{
	if (currentStep > startStep) {
		currentStep -= 1;
		fixStep();
	}
}

void StartWindow::Onbut2Clicked()
{
	if (currentStep < countStep) {
		currentStep += 1;
		fixStep();
	}
}

void StartWindow::setFullCountStep(int reg=0)
{
	countStep = 9;
	startStep = 3;
	if (reg)
	{
		PixmapString[3] = ":/mainwindow/images/mainwindow/step_1_multiaxis.png";
		TextString[3] = "Step 1: when connecting a new positioner, it is important to set the correct profile for it to avoid breakage. To do this, click the Settings button - as shown in the figure below. The procedure for loading the corresponding profile must be performed for each positioner.";
		PixmapString[7] = ":/mainwindow/images/mainwindow/step_5_multiaxis.png";
		TextString[7] = "Step 5: precise movements can be performed by setting values in the fields for moving and shifting, followed by pressing the appropriate buttons. In this case, the movement will be carried out on all axes in which the values are set. To move freely, hold Ctrl on the green field and press the left mouse button.";
	}
	currentStep = 3;
	fixStep();
}

void StartWindow::fixStep()
{
	if (b)
	{
		logo = QPixmap(PixmapString[currentStep]);
		llogo->setPixmap(logo);
		lictext->setText(TextString[currentStep]);
	}
}

QWidget* StartWindow::returnHelpWidget()
{
	return b;
}