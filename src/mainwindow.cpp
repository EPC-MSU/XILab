#include <QSettings>
#include <QMessageBox>
#include <QMenu>
#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <pagecyclicwgt.h>
#include <pageslidersetupwgt.h>
#include <cyclicsettings.h>
#include <slidersettings.h>
#include <functions.h>
#include <main.h>
#include <loggedfile.h>
#include <status.h>
#include <xsettings.h>
#include <attensettings.h>


extern bool save_configs;
QString init_fail_reason;
extern MessageLog* mlog;

MainWindow::MainWindow(QWidget *parent, QString _device_name, DeviceInterface *_devinterface)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
	t.start();
	devinterface = _devinterface;
	cs = devinterface->cs; // a shortcut for less typing
	strncpy(device_name, _device_name.toLocal8Bit().data(), 256);

    ui->setupUi(this);
	installEventFilter(this);
	QApplication::processEvents();

	inited = false;
	cyclic = false;
	prev_cyclic_state = CyclicSettings::UnitType::CYCLICUNKNOWN;
	move_to_left_bdr = false;
	move_to_right_bdr = false;
	BackToNormalState = false;
	encoder_advertised = false;
	h_bridge_complained = false;

	slider_divisor = 1;
	ui->divisorLbl->setVisible(false);
	sliderScaleDraw = new SliderScaleDraw(slider_divisor);
	ui->positionSldr->setOrientation(Qt::Horizontal);
	ui->positionSldr->setScaleDraw(sliderScaleDraw);
	ui->positionSldr->setScalePosition(QwtSlider::TopScale);
	ui->positionSldr->setBackgroundStyle(QwtSlider::Groove); // to be removed in Qwt 6.1
	ui->positionSldr->setHandleSize(QSize(30, 12));
	ui->positionSldr->setBorderWidth(1);
	ui->positionSldr->setFocusPolicy(Qt::StrongFocus);
	ui->lastUpdateValue->hide();
	ui->lastUpdateLbl->hide();

	QApplication::processEvents();	

	QObject::connect(ui->settingsBtn,  SIGNAL(clicked()),  this,  SLOT(OnSettingsBtnPressed()));
	QObject::connect(ui->chartBtn,     SIGNAL(clicked()),  this,  SLOT(OnChartBtnPressed()));
	QObject::connect(ui->scriptBtn,    SIGNAL(clicked()),  this,  SLOT(OnScriptBtnPressed()));
	QObject::connect(ui->leftBdrBtn,   SIGNAL(clicked()),  this,  SLOT(OnLeftBdrBtnPressed()));
	QObject::connect(ui->leftBtn,      SIGNAL(clicked()),  this,  SLOT(OnLeftBtnPressed()));
	QObject::connect(ui->stopBtn,      SIGNAL(clicked()),  this,  SLOT(OnStopBtnPressed()));
	QObject::connect(ui->sstpBtn,      SIGNAL(clicked()),  this,  SLOT(OnSstpBtnPressed()));
	QObject::connect(ui->rightBtn,     SIGNAL(clicked()),  this,  SLOT(OnRightBtnPressed()));
	QObject::connect(ui->rightBdrBtn,  SIGNAL(clicked()),  this,  SLOT(OnRightBdrBtnPressed()));
	QObject::connect(ui->moveBtn,      SIGNAL(clicked()),  this,  SLOT(OnMoveBtnPressed()));
	QObject::connect(ui->shiftBtn,     SIGNAL(clicked()),  this,  SLOT(OnShiftBtnPressed()));
	QObject::connect(ui->homeBtn,      SIGNAL(clicked()),  this,  SLOT(OnGoHomeBtnPressed()));
	QObject::connect(ui->cyclicBtn,    SIGNAL(clicked()),  this,  SLOT(OnCyclicBtnPressed()));
	QObject::connect(ui->zeroBtn,      SIGNAL(clicked()),  this,  SLOT(OnZeroBtnPressed()));
	QObject::connect(ui->saveLogBtn,   SIGNAL(clicked()),  this,  SLOT(OnSaveLogBtnPressed()));
	QObject::connect(ui->clearLogBtn,  SIGNAL(clicked()),  this,  SLOT(OnClearLogBtnPressed()));
    QObject::connect((QObject*)devinterface, SIGNAL(errorSignal(const char*, result_t)),    this,  SLOT(CommandErrorLog(const char*, result_t)));
    QObject::connect((QObject*)devinterface, SIGNAL(warningSignal(const char*, result_t)),  this,  SLOT(CommandWarningLog(const char*, result_t)));
	

	QApplication::processEvents();
    InitUI();
	QApplication::processEvents();

	LoadProgramConfigs(false);
	QApplication::processEvents();

	updateThread = new UpdateThread(0, this, device_name, devinterface);

	qRegisterMetaType<result_t>("result_t");
	qRegisterMetaType<measurement_t>("measurement_t");
    //обновление данных в mainwindow
	connect(updateThread, SIGNAL(done()), this, SLOT(UpdateState()));

	updateThread->start(QThread::HighestPriority);
	this->setAttribute( Qt::WA_DeleteOnClose );

	AttenSettings interf_settings;
	attenuator = new Attenuator(interf_settings);
	posit_steps_y = 0;
	prev_Pos_x = 0;
	prev_Pos_y = 0;
	prev_steps_y = 0;
	state_atten = IDLE;
}




void MainWindow::error_from_atten(const QString& text){
	Log(text, SOURCE_XILAB, LOGLEVEL_ERROR);
}

MainWindow::~MainWindow()
{
	QObject::disconnect(mlog, SLOT(InsertLine(QDateTime, QString, QString, int, LogSettings*)));
	if(inited){
		inited = false; // added because of libximc callback function
		chartDlg->enabled = false;
		chartDlg->hide();
		scriptDlg->hide();
		settingsDlg->hide();
		settingsDlg->close();
		if (save_configs) {
			SaveConfigs(settingsDlg->MakeConfigFilename());
			SaveProgramConfigs();
		}
		updateThread->wait_for_exit = true;
		disconnect(updateThread);
		while (updateThread->isRunning()) {
			msleep(20);
		}

		scriptThread->wait_for_exit = true;
		disconnect(scriptThread);
		while (scriptThread->isRunning()) {
			msleep(20);
		}

		delete chartDlg;
		delete scriptDlg;
		delete settingsDlg;

		if (devinterface->is_open()) {
			devinterface->close_device();
		}

		delete ui;
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	scriptThread->qengine->abortEvaluation();
	event->accept();
}

//вызывается только для MacOS
void MainWindow::makeNewInstance()
{
	QProcess::startDetached(QApplication::applicationFilePath());
}

void MainWindow::InitUI()
{
#ifdef __APPLE__
	//меню для MacOS
	menuBar = new QMenuBar;
	QMenu *menu = menuBar->addMenu("New instance of XILab");
	connect(menu, SIGNAL(aboutToShow()), this, SLOT(makeNewInstance()));
	setMenuBar(menuBar);
#endif

	//контролы
	moveSpinBox.setUnitType(UserUnitSettings::TYPE_COORD);
	shiftSpinBox.setUnitType(UserUnitSettings::TYPE_COORD);
	moveSpinBox.setAlignment(Qt::AlignHCenter);
	shiftSpinBox.setAlignment(Qt::AlignHCenter);
	moveSpinBox.setObjectName("moveSpinBox");
	shiftSpinBox.setObjectName("shiftSpinBox");

	ui->gridLayout_4->addWidget(&moveSpinBox, 0, 2);
	ui->gridLayout_4->addWidget(&shiftSpinBox, 1, 2);

	ui->gridLayout_4->setColumnStretch(0, 1);
	ui->gridLayout_4->setColumnStretch(1, 1);
	ui->gridLayout_4->setColumnStretch(2, 2);
	ui->gridLayout_4->setColumnStretch(3, 1);

	QFont font;
	QBrush redBrush(QColor(255, 0, 0, 255));
	QBrush greyBrush(QColor(128, 128, 128, 255));
	QBrush greenBrush(QColor(0, 208, 0, 255));
	QBrush yellowBrush(QColor(255, 212, 0, 255));
	QBrush semitransparentBrush(QColor(0, 0, 0, 12));
	QBrush clearBrush(QColor(0, 0, 0, 0));

	font.setBold(true);
	redBrush.setStyle(Qt::SolidPattern);
	greyBrush.setStyle(Qt::SolidPattern);
	greenBrush.setStyle(Qt::SolidPattern);
	yellowBrush.setStyle(Qt::SolidPattern);
	semitransparentBrush.setStyle(Qt::SolidPattern);

	palette_red.setBrush(QPalette::Active, QPalette::WindowText, redBrush);
	palette_red.setBrush(QPalette::Inactive, QPalette::WindowText, redBrush);
	palette_red.setBrush(QPalette::Disabled, QPalette::WindowText, semitransparentBrush);
	palette_red.setBrush(QPalette::Light, clearBrush); // For Windows XP & 2003

	palette_grey.setBrush(QPalette::Active, QPalette::WindowText, semitransparentBrush);
	palette_grey.setBrush(QPalette::Inactive, QPalette::WindowText, semitransparentBrush);
	palette_grey.setBrush(QPalette::Disabled, QPalette::WindowText, semitransparentBrush);
	palette_grey.setBrush(QPalette::Light, clearBrush); // For Windows XP & 2003

	palette_green.setBrush(QPalette::Active, QPalette::WindowText, greenBrush);
	palette_green.setBrush(QPalette::Inactive, QPalette::WindowText, greenBrush);
	palette_green.setBrush(QPalette::Disabled, QPalette::WindowText, semitransparentBrush);
	palette_green.setBrush(QPalette::Light, clearBrush); // For Windows XP & 2003

	palette_yellow.setBrush(QPalette::Active, QPalette::WindowText, yellowBrush);
	palette_yellow.setBrush(QPalette::Inactive, QPalette::WindowText, yellowBrush);
	palette_yellow.setBrush(QPalette::Disabled, QPalette::WindowText, semitransparentBrush);
	palette_yellow.setBrush(QPalette::Light, clearBrush); // For Windows XP & 2003

	QList<QString> names;
	QList<QString> abbr;
	QList<unsigned int> flags;
	names << "Left button" << "Right button" << "GPIO pin" << "Brake" << "Revolution sensor" << "Sync in" << "Sync out";
	abbr << "L" << "R" << "G" << "B" << "S" << "I" << "O";
	flags << STATE_BUTTON_LEFT << STATE_BUTTON_RIGHT << STATE_GPIO_LEVEL << STATE_BRAKE << STATE_REV_SENSOR << STATE_SYNC_INPUT << STATE_SYNC_OUTPUT;

	for (int i=0; i<INDICATOR_COUNT; i++) {
		btn_list[i].name = names[i];
		btn_list[i].flag = flags[i];

		QColor color;
		color.setHslF((double)i/INDICATOR_COUNT, 1.0, 0.5);
		QBrush brush(color);
		palette_rainbow[i].setBrush(QPalette::Active, QPalette::WindowText, brush);
		palette_rainbow[i].setBrush(QPalette::Inactive, QPalette::WindowText, brush);
		palette_rainbow[i].setBrush(QPalette::Disabled, QPalette::WindowText, semitransparentBrush);
		palette_rainbow[i].setBrush(QPalette::Light, clearBrush); // For Windows XP & 2003
	}

    onPix.load(":/mainwindow/images/mainwindow/ind_on.png");
    offPix.load(":/mainwindow/images/mainwindow/ind_off.png");
	redPix.load(":/mainwindow/images/mainwindow/ind_red.png");

	for (int i=0; i<INDICATOR_COUNT; i++) {
		btns[i].setFixedWidth(ST_BAR_WIDTH);
		btns[i].setAlignment(Qt::AlignHCenter);
		btns[i].setToolTip(btn_list[i].name);
		btns[i].setFont(font);
		btns[i].setPalette(palette_rainbow[i]);
		btns[i].setText(abbr[i]);
		btns[i].setDisabled(true);
		ui->statusBar->addPermanentWidget(&btns[i]);
	}

	eeprLbl.setFixedWidth(ST_PANE_WIDTH);
	eeprLbl.setAlignment(Qt::AlignHCenter);
	eeprLbl.setToolTip(tr("Lights up green when the stage is equipped with EEPROM memory chip. \nThe built - in stage profile is uploaded from the EEPROM memory chip if the EEPROM_PRECEDENCE flag(XILab Settings->About device tab) is enabled, allowing you to connect various stages to the controller with automatic setup."));
	eeprLbl.setFont(font);
	eeprLbl.setPalette(palette_green);
	eeprLbl.setText("EEPR");
	eeprLbl.setDisabled(true);
	ui->statusBar->addPermanentWidget(&eeprLbl);

	homdLbl.setFixedWidth(ST_PANE_WIDTH);
	homdLbl.setAlignment(Qt::AlignHCenter);
	homdLbl.setToolTip(tr("Lights up after successful execution of home() command meaning that relative position scale is calibrated against a hardware absolute position sensor like a limit switch. Drops after loss of calibration like harsh stop and possibly skipped steps."));
	homdLbl.setFont(font);
	homdLbl.setPalette(palette_green);
	homdLbl.setText("HOMD");
	homdLbl.setDisabled(true);
	ui->statusBar->addPermanentWidget(&homdLbl);

	wnd1Lbl.setFixedWidth(ST_PANE_WIDTH);
	wnd1Lbl.setAlignment(Qt::AlignHCenter);
	wnd1Lbl.setToolTip(tr("The winding A status has 1 of 4 statuses: \nWinding A is disconnected. \nWinding A state is unknown. \nWinding A is short - circuited. \nWinding A is connected and working properly.\nStatus is determined using statistical data while moving, taking its time, and turning this status rather useless in common applications.Therefore this function have been disabled for the moment."));
	wnd1Lbl.setFont(font);
	wnd1Lbl.setPalette(palette_grey);
	wnd1Lbl.setText("WndA");
	wnd1Lbl.setEnabled(true);
	ui->statusBar->addPermanentWidget(&wnd1Lbl);

	wnd2Lbl.setFixedWidth(ST_PANE_WIDTH);
	wnd2Lbl.setAlignment(Qt::AlignHCenter);
	wnd2Lbl.setToolTip(tr("The winding B status has 1 of 4 statuses: \nWinding B is disconnected. \nWinding B state is unknown. \nWinding B is short - circuited. \nWinding B is connected and working properly.\nStatus is determined using statistical data while moving, taking its time, and turning this status rather useless in common applications.Therefore this function have been disabled for the moment."));
	wnd2Lbl.setFont(font);
	wnd2Lbl.setPalette(palette_grey);
	wnd2Lbl.setText("WndB");
	wnd2Lbl.setEnabled(true);
	ui->statusBar->addPermanentWidget(&wnd2Lbl);

	encdLbl.setFixedWidth(ST_PANE_WIDTH);
	encdLbl.setAlignment(Qt::AlignHCenter);
	encdLbl.setToolTip(tr("Encoder state has 1 of 5 statuses: \nEncoder is absent. \nEncoder state is unknown. \nEncoder is connected and malfunctioning. \nEncoder is connected and operational but counts in other direction. \nEncoder is connected and working properly."));
	encdLbl.setFont(font);
	encdLbl.setPalette(palette_grey);
	encdLbl.setText("ENCD");
	encdLbl.setEnabled(true);
	ui->statusBar->addPermanentWidget(&encdLbl);

	pwhtLbl.setFixedWidth(ST_PANE_WIDTH);
	pwhtLbl.setAlignment(Qt::AlignHCenter);
	pwhtLbl.setToolTip(tr("Power driver overheat. Motor control is disabled until some cooldown. This should not happen in boxed versions of controller. This may happen in bare board version of controller with a custom radiator. Redesign your radiator then."));
	pwhtLbl.setFont(font);
	pwhtLbl.setPalette(palette_red);
	pwhtLbl.setText("PWHT");
	pwhtLbl.setDisabled(true);
	ui->statusBar->addPermanentWidget(&pwhtLbl);

	slipLbl.setFixedWidth(ST_PANE_WIDTH);
	slipLbl.setAlignment(Qt::AlignHCenter);
	slipLbl.setToolTip(tr("Motor slip detected. Flag is set when encoder position and step position are too far apart. You can disable the \"Position control\" flag or increase the error in the \"threshold\" field on the XILab Settings->Position control tab to stop these error from happening."));
	slipLbl.setFont(font);
	slipLbl.setPalette(palette_red);
	slipLbl.setText("SLIP");
	slipLbl.setDisabled(true);
	ui->statusBar->addPermanentWidget(&slipLbl);

	wrmLbl.setFixedWidth(ST_PANE_WIDTH);
	wrmLbl.setAlignment(Qt::AlignHCenter);
	wrmLbl.setToolTip(tr("Lights up when there is a substantial difference between stepper motor windings resistances. This usually happens with a damaged stepper motor with partially short-circuited windings. \nYou can diagnose the problem according to the instructions in our manual"));
	wrmLbl.setFont(font);
	wrmLbl.setPalette(palette_red);
	wrmLbl.setText("WRM");
	wrmLbl.setDisabled(true);
	ui->statusBar->addPermanentWidget(&wrmLbl);

	engLbl.setFixedWidth(ST_PANE_WIDTH);
	engLbl.setAlignment(Qt::AlignHCenter);
	engLbl.setToolTip(tr("Lights up red when the motor control error occurs. Motor control algorithm failure means that it can't define the correct decisions with the feedback data it receives. Single failure may be caused by mechanical problem. A repeating failure can be caused by incorrect motor settings."));
	engLbl.setFont(font);
	engLbl.setPalette(palette_red);
	engLbl.setText("ENGR");
	engLbl.setDisabled(true);
	ui->statusBar->addPermanentWidget(&engLbl);

	extLbl.setFixedWidth(ST_PANE_WIDTH);
	extLbl.setAlignment(Qt::AlignHCenter);
	extLbl.setToolTip(tr("The error is caused by the external EXTIO input signal set to cause this error in the settings (XILab Settings->EXTIO tab)."));
	extLbl.setFont(font);
	extLbl.setPalette(palette_red);
	extLbl.setText("EXTi");
	extLbl.setDisabled(true);
	ui->statusBar->addPermanentWidget(&extLbl);

	errcLbl.setFixedWidth(ST_PANE_WIDTH);
	errcLbl.setAlignment(Qt::AlignHCenter);
	errcLbl.setToolTip(tr("Command error encountered. The command received is not in the list of controller known commands. Most possible reason is the outdated firmware that can be updated in XILab Settings->About device tab->Autoupdate button."));
	errcLbl.setFont(font);
	errcLbl.setPalette(palette_red);
	errcLbl.setText("ErrC");
	errcLbl.setDisabled(true);
	ui->statusBar->addPermanentWidget(&errcLbl);
    
	errdLbl.setFixedWidth(ST_PANE_WIDTH);
	errdLbl.setAlignment(Qt::AlignHCenter);
	errdLbl.setToolTip(tr("Data integrity error encountered. The data inside command and its CRC code do not correspond, therefore data can't be considered valid. This error may be caused by EMI in UART/RS232 interface."));
	errdLbl.setFont(font);
	errdLbl.setPalette(palette_red);
	errdLbl.setText("ErrD");
	errdLbl.setDisabled(true);
	ui->statusBar->addPermanentWidget(&errdLbl);

	errvLbl.setFixedWidth(ST_PANE_WIDTH);
	errvLbl.setAlignment(Qt::AlignHCenter);
	errvLbl.setToolTip(tr("Value error encountered. The values in the command can't be applied without correction because they fall out the valid range. Corrected values were used instead of the orginal ones."));
	errvLbl.setFont(font);
	errvLbl.setPalette(palette_red);
	errvLbl.setText("ErrV");
	errvLbl.setDisabled(true);
	ui->statusBar->addPermanentWidget(&errvLbl);

	ui->movingStateLbl->setPixmap(QPixmap(":/mainwindow/images/mainwindow/state_moving.png"));
	ui->targetSpeedLbl->setPixmap(QPixmap(":/mainwindow/images/mainwindow/check.png"));
	ui->antiplayLbl->setPixmap(QPixmap(":/mainwindow/images/mainwindow/antiplay.png"));

    QPalette pal = ui->antiplayLbl->palette();
	QColor qcol = QColor(255,255,255,0);
	pal.setColor(QPalette::Background, qcol);
	ui->movingStateLbl->setPalette(pal);
	ui->targetSpeedLbl->setPalette(pal);
	ui->antiplayLbl->setPalette(pal);

	QList<QLabel*> icons;
	icons << ui->movingStateLbl << ui->targetSpeedLbl << ui->antiplayLbl;
	for (int i=0; i<icons.length(); i++) {
		icons.at(i)->setDisabled(true);
		icons.at(i)->setPalette(pal);
	}

	ui->positionSldr->setReadOnly(true);

	ui->encoderValue->setStyleSheet("QLabel { color : gray; }");

	ui->centralWidget->setAutoFillBackground(true);
	saved_bk_color = ui->centralWidget->palette().color(QPalette::Window);
	double k = 0.1;
	alarm_bk_color = QColor(min((double)255, saved_bk_color.red()*(1-k)+255*k), saved_bk_color.green()*(1-k), saved_bk_color.blue()*(1-k), 255);

	InitTable();
}

void MainWindow::Init()
{
	//когда получили device и device_name в main, инициализируем объекты
#ifdef __APPLE__ // #3084
	settingsDlg = new SettingsDlg(this, updateThread, devinterface, false);
#else
	settingsDlg = new SettingsDlg(NULL, updateThread, devinterface, false);
	settingsDlg->setWindowFlags(settingsDlg->windowFlags() | Qt::Tool);
#endif
	connect(settingsDlg, SIGNAL(SgnLog(QString, QString, int)),
			this, SLOT(Log(QString, QString, int)) );
	connect(settingsDlg->pageWgtsLst[PageADNum],SIGNAL(errorInLog(QString, QString, int)), this, SLOT(Log(QString, QString, int)) );
	LoggedFile file("");
	file.setLogSettings(settingsDlg->logStgs);

	connect(settingsDlg->pageWgtsLst[PageAboutNum], SIGNAL(exit_on_remove()), this, SLOT(OnExitBtnPressed()));

	QObject::connect(settingsDlg->pageWgtsLst[PagePidNum], SIGNAL(PIDTuneShowed()), this, SLOT(OnPIDCalibratorShow()));

	moveSpinBox.setFloatStyleAllowed(true);
	shiftSpinBox.setFloatStyleAllowed(true);
	moveSpinBox.setTempChangeAllowed(false);
	shiftSpinBox.setTempChangeAllowed(false);
	moveSpinBox.setSerial(settingsDlg->controllerStgs->serialnumber);
	shiftSpinBox.setSerial(settingsDlg->controllerStgs->serialnumber);

	positionerNeedsUpdate = true;
	QApplication::processEvents();
	QString title = QString(settingsDlg->controllerStgs->device_info.ManufacturerId) + toStr(settingsDlg->controllerStgs->serialnumber);
#ifdef __APPLE__ // #3084
	chartDlg = new ChartDlg(this, settingsDlg, title, false);
#else
	chartDlg = new ChartDlg(NULL, settingsDlg, title, false);
	chartDlg->setWindowFlags(chartDlg->windowFlags() | Qt::WindowMaximizeButtonHint | Qt::Tool);
#endif
	chartDlg->setHardware(settingsDlg->getCurrentMotorType());
	connect(settingsDlg, SIGNAL(SgnHardwareChanged(unsigned int)), chartDlg, SLOT(setHardware(unsigned int)));
	QApplication::processEvents();

#ifdef __APPLE__ // #3084
	scriptDlg = new ScriptDlg(this, settingsDlg->motorStgs, title);
#else
	scriptDlg = new ScriptDlg(NULL, settingsDlg->motorStgs, title);
	scriptDlg->setWindowFlags(scriptDlg->windowFlags() | Qt::WindowMaximizeButtonHint | Qt::Tool);
#endif
	connect(scriptDlg, SIGNAL(finished(int)),				this,	SLOT(OnScriptStopBtnPressed()));
	connect(scriptDlg, SIGNAL(start()),						this,	SLOT(OnScriptStartBtnPressed()));
	connect(scriptDlg, SIGNAL(stop(bool)),					this,	SLOT(OnScriptStopBtnPressed(bool)));
	connect(scriptDlg, SIGNAL(script_exception(QString)),	this,	SLOT(ScriptExceptionLog(QString)));
	connect(ui->stopBtn, SIGNAL(clicked()), scriptDlg, SLOT(OnStopBtnClicked()));

	QApplication::processEvents();

	scriptThread = new ScriptThread(this, devinterface);
	p_scriptThread = scriptThread;

	scriptThread->start(QThread::IdlePriority);
	QApplication::processEvents();

	//при изменении настроек (возможно изменились настройки ограничителей), обновляем маркеры ограничителей
	QObject::connect(settingsDlg, SIGNAL(SgnChangeSettings()), 
					 chartDlg,	  SLOT(updateSettings()));
	
	//при изменении критических настроек
	QObject::connect(settingsDlg, SIGNAL(SgnChangeCritSettings()), 
					 this,	      SLOT(critSettingsChanged()));

	//при изменении настроек пользовательской калибровки
	QObject::connect(settingsDlg, SIGNAL(SgnUpdateCalibration(UserUnitSettings, unsigned int)),
				devinterface->cs, SLOT(updateCalibration(UserUnitSettings, unsigned int)));

    //чтобы изменить  интерфейс с настройками
	connect(settingsDlg, SIGNAL(clickedChangeInterface(AttenSettings,bool)),
				this, SLOT(changeInterface(AttenSettings,bool)));


	//загрузка из конфига DCxxx.cfg, при его отсутствии - из xilabdefault.cfg (сначала по пути инсталляции, потом в директории программы)
	if (QFile::exists(settingsDlg->MakeConfigFilename()))
		LoadConfigs(settingsDlg->MakeConfigFilename());
	else
		if (QFile::exists(DefaultConfigFilename()))
			LoadConfigs(DefaultConfigFilename());
		else
			if (QFile::exists(DefaultLocalConfigFilename()))
				LoadConfigs(DefaultLocalConfigFilename());
			else {
				throw my_exception("Couldn't find either saved or default configuration file.");
			}
	QApplication::processEvents();

	// #23168 change all spin boxes maximum values
	Q_FOREACH(QWidget* item, settingsDlg->pageWgtsLst) {
		Q_FOREACH(QSpinBox* sbox, item->findChildren<QSpinBox*>()) {
			if (dynamic_cast<StepSpinBox*>(sbox) == nullptr) { // we do not touch stepspinboxes, because now they can contain values in userunits: #17722
				sbox->setRange(0, INT_MAX);
			}
		}
	}
    
//	AttenSettings interf_settings; 
	((PageCyclicWgt*)settingsDlg->pageWgtsLst[PageCyclicNum])->FromUiToClass();
	((PageUiConfigWgt*)settingsDlg->pageWgtsLst[PageUiConfigNum])->FromUiToClass();
	((PageIntrfSettingsWgt*)settingsDlg->pageWgtsLst[PageIntrfSettingsNum])->FromUiToClass();
	((PageSliderSetupWgt*)settingsDlg->pageWgtsLst[PageSliderSetupNum])->FromUiToClass();
	((PageUserUnitsWgt*)settingsDlg->pageWgtsLst[PageUserUnitsNum])->FromUiToClass();
	  
	changeInterface(*settingsDlg->intStgs);

	settingsDlg->UpdateCalibration();

	//загрузка в GraphSettings
	((PageGraphCommonWgt*)settingsDlg->pageWgtsLst[PageGraphNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphPosNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphSpeedNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphEncoderNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphFlagsNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphEnVoltNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphEnCurrNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphPwrVoltNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphPwrCurrNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphUsbVoltNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphUsbCurrNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphCurrANum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphCurrBNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphCurrCNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphVoltANum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphVoltBNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphVoltCNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphPwmNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphTempNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphJoyNum])->FromUiToClass();
	((PageGraphWgt*)settingsDlg->pageWgtsLst[PageGraphPotNum])->FromUiToClass();
	QApplication::processEvents();

	// Connect disable signal to stage pages grayout function
	QObject::connect(this, SIGNAL(SgnDisableStagePages(bool)), ((PagePositionerNameWgt*)settingsDlg->pageWgtsLst[PagePositionerNameNum]), SLOT(SetDisabled(bool)));
	QObject::connect(this, SIGNAL(SgnDisableStagePages(bool)), ((PageStageWgt*)settingsDlg->pageWgtsLst[PageStageNum]), SLOT(SetDisabled(bool)));
	QObject::connect(this, SIGNAL(SgnDisableStagePages(bool)), ((PageStageDCMotorWgt*)settingsDlg->pageWgtsLst[PageStageDCMotorNum]), SLOT(SetDisabled(bool)));
	QObject::connect(this, SIGNAL(SgnDisableStagePages(bool)), ((PageStageEncoderWgt*)settingsDlg->pageWgtsLst[PageStageEncoderNum]), SLOT(SetDisabled(bool)));
	QObject::connect(this, SIGNAL(SgnDisableStagePages(bool)), ((PageStageHallsensorWgt*)settingsDlg->pageWgtsLst[PageStageHallsensorNum]), SLOT(SetDisabled(bool)));
	QObject::connect(this, SIGNAL(SgnDisableStagePages(bool)), ((PageStageGearWgt*)settingsDlg->pageWgtsLst[PageStageGearNum]), SLOT(SetDisabled(bool)));
	QObject::connect(this, SIGNAL(SgnDisableStagePages(bool)), ((PageStageAccessoriesWgt*)settingsDlg->pageWgtsLst[PageStageAccessoriesNum]), SLOT(SetDisabled(bool)));
	QApplication::processEvents();
	emit SgnDisableStagePages(true);

	chartDlg->InitUI(settingsDlg->graphStgs);
	QApplication::processEvents();

	scriptDlg->InitUI();
	QApplication::processEvents();

	scriptStartBtn = scriptDlg->findChild<QPushButton*>("startBtn");
	scriptStopBtn = scriptDlg->findChild<QPushButton*>("stopBtn");
	scriptLoadBtn = scriptDlg->findChild<QPushButton*>("loadBtn");
	scriptSaveBtn = scriptDlg->findChild<QPushButton*>("saveBtn");
	codeEdit = scriptDlg->findChild<QTextEdit*>("codeEdit");
	QApplication::processEvents();

	settingsDlg->setStyles(true); // required to initialize moveto and shifton stepspinboxes with correct units
	QApplication::processEvents();

	connect(updateThread,	SIGNAL(done()), 
			chartDlg,		SLOT(InsertMeasure()));


	QApplication::processEvents();
	
	//применение настроек графиков
	connect(settingsDlg,	SIGNAL(SgnChangeSettings()),
			chartDlg,		SLOT(ReinitPlots()));

	QApplication::processEvents();

	LoadProgramConfigs(true);			//загрузка Key

	mlog->setSerial(settingsDlg->controllerStgs->serialnumber);
	QApplication::processEvents();

	
	// засеривание нереализованных функций
	exitBox = new InfoBox(this);
	exitBox->setIcon(QMessageBox::Information);
	exitBox->setButtons(QDialogButtonBox::Cancel);
	exitBox->setText("Stopping, please wait...");
	exitBox->setClosingOnReject(false);

	infoBox.setButtonsVisible(true);
	infoBox.setButtons(QDialogButtonBox::Cancel);
	infoBox.setMovieVisible(true);
	infoBox.setIcon(QMessageBox::Information);
	connect(&infoBox, SIGNAL(cancelled()), this, SLOT(stopByCancel()));
	infoBox.setText("Wait for motion completion");

	previousStateIsAlarm = false;
	exiting = false;
	inited = true;
}

int mod_atten_func1(int new_per, int old_per,int per_turn) {
	//not correct steps
	if(!per_turn)
		return 0;
	int razn = new_per - old_per;
	if (razn < 0)
		return (per_turn + razn)%per_turn;
	else 
		return razn%per_turn;
}

int mod_atten_func2(int new_per, int old_per,int per_turn) {
	//not correct steps
	if(!per_turn)
		return 0;
	int razn = new_per - old_per;
	if (razn > 0)
		return (per_turn - razn)%per_turn;
	else {
		int rezult = -razn%per_turn;
		return rezult;
	}
}

void MainWindow::AttenMove(){
    int turn = settingsDlg->motorStgs->engine.StepsPerRev;
	if (!turn) {
		return;
	}
	attenuator->should_move = true;
	state_atten = START_CALIBRATE;
}

void MainWindow::AttenCalibrate() {
	attenuator->setCalibrationStatus(Calibration::IN_PROGRESS);
	attenuator->should_move = false;
	state_atten = START_CALIBRATE;
}

void MainWindow::critSettingsChanged()
{
	Log(QString("Essential settings changed: possible microstep rounding error and/or incorrect movement settings. Please verify all settings are correct before issuing any movement commands."), SOURCE_XILAB, LOGLEVEL_WARNING);
}

inline void clearEditableFlag(QTableWidgetItem *item)
{
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
}

void MainWindow::UpdateLogTable()
{
	double tick1, tick2;
	tick2 = tick1 = t.getElapsedTimeInMilliSec();
	bool do_autoscroll = (ui->logEdit->verticalScrollBar()->sliderPosition() == ui->logEdit->verticalScrollBar()->maximum());

	// Update log
	LogItem next_item;
	QString message_old = "";
	int counter_mess = 1;
	while (mlog->pop(&next_item) && (tick2 - tick1 < 50) ) { // gathers data for 50ms max (out of 100ms limit for a single update)
		tick2 = t.getElapsedTimeInMilliSec();
		QDateTime datetime = next_item.datetime;
		QString message = next_item.message;
		
		QString source = next_item.source;
		int loglevel = next_item.loglevel;

		QString date = datetime.toString("yyyy.MM.dd");
		QString time = datetime.toString("hh:mm:ss.zzz");
		LogSettings* ls = settingsDlg->logStgs;

		if ((source == SOURCE_XILAB && loglevel <= ls->xll_index) || 
			(source == SOURCE_SCRIPT && loglevel <= ls->sll_index) ||
			(source == SOURCE_LIBRARY && loglevel <= ls->lll_index)) {

			int row = ui->logEdit->rowCount();
			if (message_old != message) {
				ui->logEdit->insertRow(row);
				ui->logEdit->setRowCount(row + 1);
				message_old = message;
				counter_mess = 1;
			}
			else
			{
				row--;
				counter_mess++;
			}

			QColor color = QColor("black");
			switch (loglevel)
			{
				case LOGLEVEL_ERROR:	color = QColor("#FF0000"); break;
				case LOGLEVEL_WARNING:	color = QColor("#B0A000"); break;
				case LOGLEVEL_INFO: 	color = QColor("#008000"); break;
				case LOGLEVEL_DEBUG:	color = QColor("#0000FF"); break;
			}

			QTableWidgetItem *item0 = new QTableWidgetItem(date);
			clearEditableFlag(item0);
			ui->logEdit->setItem(row, 0, item0);
			QTableWidgetItem *item1 = new QTableWidgetItem(time);
			clearEditableFlag(item1);
			ui->logEdit->setItem(row, 1, item1);
			QTableWidgetItem *item2 = new QTableWidgetItem(source);
			clearEditableFlag(item2);
			ui->logEdit->setItem(row, 2, item2);
			QString count = "";
			if (counter_mess > 1)
				count = " [" + QString::number(counter_mess) + "]";
			QTableWidgetItem *item3 = new QTableWidgetItem((QString) (message + count));
			clearEditableFlag(item3);
			item3->setTextColor(color);
			ui->logEdit->setItem(row, 3, item3);

			ui->logEdit->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
			ui->logEdit->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
			ui->logEdit->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);
			ui->logEdit->horizontalHeader()->setResizeMode(3, QHeaderView::Stretch);
		}
	}
	if (do_autoscroll)
		ui->logEdit->scrollToBottom();
}

void MainWindow::InitTable()
{
	ui->logEdit->setColumnCount(4);
	headerLabels.clear();
	headerLabels << QString("Date") << QString("Time") << QString("Source") << QString("Message");
	ui->logEdit->setHorizontalHeaderLabels(headerLabels);
	ui->logEdit->verticalHeader()->setVisible(false);

	ui->logEdit->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

	QFontMetrics fm (ui->logEdit->font());
	int h = fm.height() + 2;
	ui->logEdit->verticalHeader()->setDefaultSectionSize( h );

	ui->logEdit->setStyleSheet("QTableWidget { gridline-color: #F0F0F0 }");
}

void MainWindow::changeInterface(AttenSettings settings, bool correct) {
	delete attenuator;
    attenuator = new Attenuator(settings); 
	int turn = settingsDlg->motorStgs->engine.StepsPerRev;
	if (settings.skinType == GeneralSkin) {
		attenuator->hide();
		ui->verticalLayout_3->addWidget(ui->mainBox);
		ui->homeBtn->setEnabled(true);
		ui->cyclicBtn->setEnabled(true);
		ui->zeroBtn->setEnabled(true);
		ui->mainBox->show();
		return;
	}
	else {
		ui->mainBox->hide();
		ui->verticalLayout_3->addWidget(attenuator);
		ui->homeBtn->setDisabled(true);
		ui->cyclicBtn->setDisabled(true);
		ui->zeroBtn->setDisabled(true);
		attenuator->show();
		//not correct steps per turn
		if (!correct || !turn) {
			mlog->InsertLine(QDateTime::currentDateTime(), "Attenuator can't move:please enter correct \"steps per turn\" in Settings->Device Configuration->Stepper motor", SOURCE_XILAB, LOGLEVEL_ERROR, NULL);
			return;
		}
	}


	int last_Pos_x = prev_Pos_x;
	int last_Pos_y = prev_Pos_y;
	attenuator->clicked(last_Pos_y+1);//to shift on first wheel
	attenuator->clicked(last_Pos_x+FILTER_COUNT+1);//to shift on second wheel
    connect(attenuator,SIGNAL(ChangePosition()),this,SLOT(AttenMove()));
	connect(attenuator,SIGNAL(Calibrate()),this,SLOT(AttenCalibrate()));
    connect(attenuator,SIGNAL(error_get(const QString& )),this,SLOT(error_from_atten(const QString&)));
}

void MainWindow::UpdateState()
{
	if (inited && exiting) {
		unsigned int status_MvCmdSts = cs->status().MvCmdSts;
		unsigned int status_MvSts = cs->status().MoveSts;
		/*
		 * Motor stopped OK or stopping is not necessary (move status is 0, motor does nothing)
		 */
		if (status_MvCmdSts == MVCMD_SSTP || (status_MvSts == 0)) { // let's exit
			if (exitBox != NULL) {
				if (exitBox->isVisible()) {
					result_t result = devinterface->command_power_off(); // power off the device
					if (result != result_ok) {
						Log("Error sending power off command on exit.", SOURCE_XILAB, LOGLEVEL_WARNING);
						return;
					}
					delete exitBox;
					exitBox = NULL;
					close();
				} else {
					exiting = false;
				}
			}
		}
		else if (status_MvCmdSts == MVCMD_SSTP + MVCMD_RUNNING) { // Stopping, let's do nothing
			;
		}
		else { // exiting == true, yet we're doing it wrong; let's error
			Log(QString("Exit aborted because of external command interrupt (joystick, sync in, etc.)."), SOURCE_XILAB, LOGLEVEL_WARNING);
			exiting = false;
			exitBox->hide();
		}
	}
	if (!inited){
		return;
	}
	// Let's show time since last update
	if (ui->lastUpdateValue->isVisible())
		ui->lastUpdateValue->setText(toStr(cs->lastUpdateTime().msecsTo(QDateTime::currentDateTimeUtc())) + " ms");

	UpdateLogTable();
	if(!cs->connect()){
		ClearState();
		QApplication::processEvents();
		return;
	}

	QString wndTitle, title;
	if ((cs->status().Flags & STATE_EEPROM_CONNECTED) && cs->connect()) {
		if (positionerNeedsUpdate) {
			result_t result = devinterface->get_stage_name(&settingsDlg->stageStgs->stage_name);
			positionerNeedsUpdate = (result != result_ok);
			if (settingsDlg->isVisible() == true) {
				settingsDlg->StageSettingsFromDeviceToClassToUi();
				emit SgnDisableStagePages(false);
			} else {
				settingsDlg->stageStgs->need_load = true;
			}
		}
	} else {
		if (positionerNeedsUpdate == false) {
			settingsDlg->stageStgs->stage_name.PositionerName[0] = '\0'; // erases username on disconnect
			emit SgnDisableStagePages(true);
			if (settingsDlg->isVisible() == false)
				settingsDlg->stageStgs->need_load = false;
		}
		positionerNeedsUpdate = true;
	}

	if ((strlen(settingsDlg->stageStgs->stage_name.PositionerName) > 0) && (cs->status().Flags & STATE_EEPROM_CONNECTED))
		title.append("[" + QString::fromUtf8(settingsDlg->stageStgs->stage_name.PositionerName) + "]");
	else if (strlen(settingsDlg->motorStgs->name.ControllerName) > 0)
		title.append(QString::fromUtf8(settingsDlg->motorStgs->name.ControllerName));
	else 
		title.append(QString(settingsDlg->controllerStgs->device_info.ManufacturerId)
						   + toStr(settingsDlg->controllerStgs->serialnumber));
	wndTitle = "XILab " + QString(XILAB_VERSION)+ " - " + title;
	settingsDlg->setWindowTitle("Settings " + title);
	scriptDlg->setWindowTitle("Scripts " + title);
	chartDlg->setWindowTitle("Charts " + title);

	if(!cs->connect()){
		wndTitle += tr(" (disconnected)");
		setWindowTitle(wndTitle);
		if (!deviceLost) {
			Log("Device lost.", SOURCE_XILAB, LOGLEVEL_WARNING);
			deviceLost = true;
		}
		ui->positionSldr->setEnabled(false);
		ClearState();
		return;
	}
	bool currentStateIsAlarm = ((cs->status().Flags & STATE_ALARM) != 0);
	unsigned long currentAlarmFlags = (cs->status().Flags | ~STATE_ALARM);
	QPalette mainpal = ui->centralWidget->palette();
	mainpal.setColor(QPalette::Window, (currentStateIsAlarm ? alarm_bk_color : saved_bk_color));
	ui->centralWidget->setPalette(mainpal);
	QFont stopfont = ui->stopBtn->font();
	stopfont.setBold(currentStateIsAlarm);
	ui->stopBtn->setFont(stopfont);

	if (currentStateIsAlarm && !previousStateIsAlarm) {
		Log("ALARM Signal: Press \"Stop\" button to continue.", SOURCE_XILAB, LOGLEVEL_ERROR);
		if (cs->status().Flags & STATE_POWER_OVERHEAT)
			Log("Power driver overheat. Motor control is disabled until some cooldown. This should not happen in boxed versions of controller. This may happen in bare board version of controller with a custom radiator. Redesign your radiator then", SOURCE_XILAB, LOGLEVEL_ERROR);
		if (cs->status().Flags & STATE_WINDING_RES_MISMATCH)
			Log("Lights up when there is a substantial difference between stepper motor windings resistances. This usually happens with a damaged stepper motor with partially short-circuited windings.", SOURCE_XILAB, LOGLEVEL_ERROR);
		if (cs->status().Flags & STATE_ENGINE_RESPONSE_ERROR)
			Log("Lights up red when the motor control error occurs. Motor control algorithm failure means that it can't define the correct decisions with the feedback data it receives. Single failure may be caused by mechanical problem. A repeating failure can be caused by incorrect motor settings.", SOURCE_XILAB, LOGLEVEL_ERROR);
		if (cs->status().Flags & STATE_ERRC)
			Log("Command error encountered. The command received is not in the list of controller known commands. Most possible reason is the outdated firmware that can be updated in XILab Settings->About device tab->Autoupdate button", SOURCE_XILAB, LOGLEVEL_ERROR);
		if (cs->status().Flags & STATE_ERRD)
			Log("Data integrity error encountered. The data inside command and its CRC code do not correspond, therefore data can't be considered valid. This error may be caused by EMI in UART/RS232 interface", SOURCE_XILAB, LOGLEVEL_ERROR);
		if (cs->status().Flags & STATE_ERRV)
			Log("Value error encountered. The values in the command can't be applied without correction because they fall out the valid range. Corrected values were used instead of the orginal ones", SOURCE_XILAB, LOGLEVEL_ERROR);
	}

	// Feature #54146 We output a message about a short-term alarm to the xilab log
	if (!currentStateIsAlarm && previousStateIsAlarm) {
		if (!(settingsDlg->motorStgs->secure.Flags & ALARM_FLAGS_STICKING) && (previousAlarmFlags & (STATE_SECUR | ~STATE_ALARM))) {
			Log("A failure occurred, but the reason of the failure seems to be nonexistent already. You can store even shortest failure conditions in the alarm flags section making them visible afterwards by enabling the \"Sticky Alarm flags\" in XILab Settings->Maximum ratings", SOURCE_XILAB, LOGLEVEL_WARNING);
		}
	}

	previousStateIsAlarm = currentStateIsAlarm;
	previousAlarmFlags = currentAlarmFlags;
	


	if(devinterface->getMode() == BOOTLOADER_MODE)
	{
		wndTitle += tr(" (bootloader v%1.%2.%3)").arg(settingsDlg->controllerStgs->bootloader_version.major).arg(settingsDlg->controllerStgs->bootloader_version.minor).arg(settingsDlg->controllerStgs->bootloader_version.release);
		setWindowTitle(wndTitle);
		ui->positionSldr->setEnabled(false);
		ClearState(devinterface->getMode());

		// Getting error flags
		errcLbl.setEnabled(cs->status().Flags & STATE_ERRC);
		errdLbl.setEnabled(cs->status().Flags & STATE_ERRD);
		errvLbl.setEnabled(cs->status().Flags & STATE_ERRV);

		return;
	}
	
	if(BackToNormalState)
	{
		BackToNormalState = false;
		settingsDlg->motorStgs->LoadFirstInfo(); // needed by NormalState's twin spinbox setStepFactor
		NormalState();
		LoadPosition();
		settingsDlg->force_save_all = true;

		settingsDlg->CheckProfileFwHwVersions();
	}

	setWindowTitle(wndTitle);

    QString value;
    QFont font;
    QBrush redBrush(QColor(255, 0, 0, 255));
    QBrush blueBrush(QColor(0, 0, 255, 255));
    QPalette palette_over, palette_under;

	redBrush.setStyle(Qt::SolidPattern);
	blueBrush.setStyle(Qt::SolidPattern);
	palette_over.setBrush(QPalette::Active, QPalette::WindowText, redBrush);
	palette_over.setBrush(QPalette::Inactive, QPalette::WindowText, redBrush);
	palette_under.setBrush(QPalette::Active, QPalette::WindowText, blueBrush);
	palette_under.setBrush(QPalette::Inactive, QPalette::WindowText, blueBrush);


	if(settingsDlg->sliderStgs->loadfromdevice){
		left_border = settingsDlg->motorStgs->edges.LeftBorder;
		right_border = settingsDlg->motorStgs->edges.RightBorder;
	}
	else{
		left_border = settingsDlg->sliderStgs->left;
		right_border = settingsDlg->sliderStgs->right;
	}

	// Calibrated slider
	double curpos;
	if (cs->useCalibration()) {
		curpos = cs->statusCalb().CurPosition;
		left_border *= settingsDlg->uuStgs->getUnitPerStep();
		right_border *= settingsDlg->uuStgs->getUnitPerStep();
	} else {
		curpos = cs->status().CurPosition + (double)cs->status().uCurPosition / settingsDlg->motorStgs->getStepFrac();
	}
	slider_borders(left_border, right_border, curpos, &slider_left, &slider_right);

	slider_divisor = pow(10, max(floor(log10((double)max(((qAbs(slider_left)-1)/100), ((qAbs(slider_right)-1)/100)))), 0.0));
	if (slider_divisor != slider_divisor) // see IEEE 754
		slider_divisor = 1;
	ui->divisorLbl->setVisible(true);

	SetLblDivisor(slider_divisor);
	sliderScaleDraw->setDivisor(slider_divisor);
	ui->positionSldr->setScale(slider_left, slider_right);
	ui->positionSldr->setRange(slider_left, slider_right, 1.0, 0); // to be removed in Qwt 6.1
	ui->positionSldr->setScaleMaxMajor(8);
	ui->positionSldr->setScaleMaxMinor(10);
	ui->positionSldr->setValue(curpos);
	ui->positionSldr->setEnabled(settingsDlg->motorStgs->edges.LeftBorder <= settingsDlg->motorStgs->edges.RightBorder);

    // Getting current position
	value = settingsDlg->getFormattedPosition();
    ui->positionValue->setText(value);
    ui->encoderValue->setText(QString("%1").arg(cs->status().EncPosition));
	ui->encoderValue->setVisible(settingsDlg->sliderStgs->showsecondary);

    // Getting current speed
	value = settingsDlg->getFormattedSpeed();
	ui->speedValue->setText(value);

	//target speed и antiplay
	if(cs->status().MoveSts & MOVE_STATE_MOVING) {
		ui->movingStateLbl->setEnabled(true);
		ui->movingStateLbl->setToolTip("State is moving.");
	} else {
		ui->movingStateLbl->setEnabled(false);
		ui->movingStateLbl->setToolTip("");
	}

	if(cs->status().MoveSts & MOVE_STATE_TARGET_SPEED) {
		ui->targetSpeedLbl->setEnabled(true);
		ui->targetSpeedLbl->setToolTip("Target speed reached.");
	} else {
		ui->targetSpeedLbl->setEnabled(false);
		ui->targetSpeedLbl->setToolTip("");
	}

	if(cs->status().MoveSts & MOVE_STATE_ANTIPLAY) {
		ui->antiplayLbl->setEnabled(true);
		ui->antiplayLbl->setToolTip("Backlash compensation active.");
	} else {
		ui->antiplayLbl->setEnabled(false);
		ui->antiplayLbl->setToolTip("");
	}

    // Getting voltage
	font.setBold(false);
	value = QString(tr("%1 V")).arg(cs->status().Upwr/100.);
	ui->pwrVoltageValue->setPalette(ui->pwrVoltageLbl->palette());

    if (cs->status().Flags & STATE_OVERLOAD_POWER_VOLTAGE)
    {
        font.setBold(true);
        ui->pwrVoltageValue->setPalette(palette_over);
    }
	else if (cs->status().Flags & STATE_LOW_POWER_VOLTAGE)
	{
		font.setBold(true);
		ui->pwrVoltageValue->setPalette(palette_under);
	}
    else
    {
        font.setBold(false);
        ui->pwrVoltageValue->setPalette(ui->pwrVoltageLbl->palette());
    }
	value = QString(tr("%1 V")).arg(cs->status().Upwr/100.);
    ui->pwrVoltageValue->setFont(font);
    ui->pwrVoltageValue->setText(value);

    // Getting current at motor
    if (cs->status().Flags & STATE_OVERLOAD_POWER_CURRENT)
    {
        font.setBold(true);
        ui->pwrCurrentValue->setPalette(palette_over);
    }
    else
    {
        font.setBold(false);
        ui->pwrCurrentValue->setPalette(ui->pwrCurrentLbl->palette());
    }
	value = QString(tr("%1 mA")).arg(cs->status().Ipwr);
    ui->pwrCurrentValue->setFont(font);
    ui->pwrCurrentValue->setText(value);

	// #9629, don't show deprecated fields for new controllers
#ifndef SERVICEMODE
	if (settingsDlg->controllerStgs->treatAs8SMC5())
	{
		ui->usbVoltageValue->hide();
		ui->usbCurrentValue->hide();
		ui->usbVoltageLbl->hide();
		ui->usbCurrentLbl->hide();
	}
	else
#endif
	{
		// Getting usb voltage
		if (cs->status().Flags & STATE_OVERLOAD_USB_VOLTAGE)
		{
			font.setBold(true);
			ui->usbVoltageValue->setPalette(palette_over);
		}
		else if (cs->status().Flags & STATE_LOW_USB_VOLTAGE)
		{
			font.setBold(true);
			ui->usbVoltageValue->setPalette(palette_under);
		}
		else
		{
			font.setBold(false);
			ui->usbVoltageValue->setPalette(ui->usbVoltageLbl->palette());
		}
		value = QString(tr("%1 V")).arg(cs->status().Uusb/100.0);
		ui->usbVoltageValue->setFont(font);
		ui->usbVoltageValue->setText(value);

		// Getting usb current
		if (cs->status().Flags & STATE_OVERLOAD_USB_CURRENT)
		{
			font.setBold(true);
			ui->usbCurrentValue->setPalette(palette_over);
		}
		else
		{
			font.setBold(false);
			ui->usbCurrentValue->setPalette(ui->usbCurrentLbl->palette());
		}
		value = QString(tr("%1 mA")).arg(cs->status().Iusb);
		ui->usbCurrentValue->setFont(font);
		ui->usbCurrentValue->setText(value);

		ui->usbVoltageValue->setEnabled(true);
		ui->usbCurrentValue->setEnabled(true);
		ui->usbVoltageLbl->setEnabled(true);
		ui->usbCurrentLbl->setEnabled(true);
	}

    // Getting current temperature
	if (cs->status().Flags & STATE_CONTROLLER_OVERHEAT)
	{
		font.setBold(true);
		ui->temperatureValue->setPalette(palette_over);
	}
	else
	{
		font.setBold(false);
		ui->temperatureValue->setPalette(ui->temperatureLbl->palette());
	}
	value = QString("%1 <span style=\"vertical-align:super;\">0</span>C").arg(cs->status().CurT/10., 0, 'f', 1);
	ui->temperatureValue->setFont(font);
	ui->temperatureValue->setText(value);
	ui->temperatureLbl->setEnabled(true);
	ui->temperatureValue->setEnabled(true);

    // Getting device state
	switch (cs->status().MvCmdSts & MVCMD_NAME_BITS)
	{
		case MVCMD_MOVE : value = tr("Move to position"); break;
		case MVCMD_MOVR : value = tr("Shift on offset"); break;
		case MVCMD_LEFT : value = tr("Move left"); break;
		case MVCMD_RIGHT: value = tr("Move right"); break;
		case MVCMD_STOP : value = tr("Stop"); break;
		case MVCMD_HOME : value = tr("Homing"); break;
		case MVCMD_LOFT : value = tr("Loft"); break;
		case MVCMD_SSTP : value = tr("Soft stop"); break;
		default         : value = tr("Unknown");
	}
	font.setBold(cs->status().MvCmdSts & MVCMD_RUNNING);

	QBrush blackBrush(QColor(0, 0, 0, 255));
	QBrush clearBrush(QColor(0, 0, 0, 0));
	QPalette pal = ui->deviceLbl->palette();
	if (cs->status().MvCmdSts & MVCMD_ERROR) {
		pal.setBrush(QPalette::Active, QPalette::WindowText, redBrush);
		pal.setBrush(QPalette::Inactive, QPalette::WindowText, redBrush);
		pal.setBrush(QPalette::Light, clearBrush); // For Windows XP & 2003
	}
	else {
		pal.setBrush(QPalette::Active, QPalette::WindowText, blackBrush);
		pal.setBrush(QPalette::Inactive, QPalette::WindowText, blackBrush);
		pal.setBrush(QPalette::Light, clearBrush); // For Windows XP & 2003
	}
	ui->deviceValue->setPalette(pal);

	ui->deviceValue->setFont(font);
	ui->deviceValue->setText(value);

	ui->pwmValue->setHidden(true);
	ui->pwmLbl->setHidden(true);

    // Getting motor power state
	switch (cs->status().PWRSts)
	{
		case PWR_STATE_OFF		: value = tr("Off"); break;
		case PWR_STATE_NORM		: value = tr("Norm"); break;
		case PWR_STATE_REDUCT	: value = tr("Reduc"); break;
		case PWR_STATE_MAX		: value = tr("Max"); break;
		case PWR_STATE_UNKNOWN	: value = tr("Uknwn"); break;
		default                 : value = tr("Unknown");
	}
	ui->pwrValue->setText(value);
	ui->pwrValue->setEnabled(true);
	ui->pwrLbl->setEnabled(true);

    // Getting edges
    if (cs->status().GPIOFlags & STATE_LEFT_EDGE) ui->lBdrInd->setPixmap(onPix);
    else ui->lBdrInd->setPixmap(offPix);

    if (cs->status().GPIOFlags & STATE_RIGHT_EDGE) ui->rBdrInd->setPixmap(onPix);
    else ui->rBdrInd->setPixmap(offPix);

	if (cs->status().Flags & STATE_BORDERS_SWAP_MISSET) {
		ui->lBdrInd->setPixmap(redPix);
		ui->rBdrInd->setPixmap(redPix);
	}

	// EXTIO stop (see #17427)
	// (3 conditions: hight level, stop on hight level, extio mode is input)
	// Bug #57092 Spontaneous reset of alarm
	// | EXTIO_SETUP_MODE_IN_BITS added to exclude EXTIO_SETUP_MODE_IN_ALARM and EXTIO_SETUP_MODE_IN_MOVR
	if ((cs->status().GPIOFlags & STATE_GPIO_LEVEL) && ((settingsDlg->motorStgs->extio.EXTIOModeFlags | EXTIO_SETUP_MODE_IN_BITS) == EXTIO_SETUP_MODE_IN_STOP) && !(settingsDlg->motorStgs->extio.EXTIOSetupFlags & EXTIO_SETUP_OUTPUT))
	{
		cyclic = false;
		devinterface->command_stop();
	}

	//Move to border & Limit switches
	if (move_to_left_bdr && (cs->status().GPIOFlags & STATE_LEFT_EDGE)){
		move_to_left_bdr = false;
		devinterface->command_stop();				//не вызываем нажатие кнопки Stop, т.к. выключится Cyclic
	}
	if (move_to_right_bdr && (cs->status().GPIOFlags & STATE_RIGHT_EDGE)){
		move_to_right_bdr = false;
		devinterface->command_stop();				//не вызываем нажатие кнопки Stop, т.к. выключится Cyclic
	}
	if ((cs->status().Flags & STATE_H_BRIDGE_FAULT) && h_bridge_complained == false) {
		Log("Power driver failure detected. It is either critically overheated or there is internal damage in the controller.", SOURCE_XILAB, LOGLEVEL_ERROR);
		h_bridge_complained = true;
	} else if (!(cs->status().Flags & STATE_H_BRIDGE_FAULT) && h_bridge_complained == true) {
		h_bridge_complained = false;
	}
	// Getting sync buffer length
	ui->cmdBufValue->setText(toStr(cs->status().CmdBufFreeSpace));

    // Getting error flags
	errcLbl.setEnabled(cs->status().Flags & STATE_ERRC);
	errdLbl.setEnabled(cs->status().Flags & STATE_ERRD);
	errvLbl.setEnabled(cs->status().Flags & STATE_ERRV);
	//Getting other flags from GETS structure - feature #1295
	slipLbl.setEnabled(cs->status().Flags & STATE_CTP_ERROR);
	pwhtLbl.setEnabled(cs->status().Flags & STATE_POWER_OVERHEAT);
	homdLbl.setEnabled(cs->status().Flags & STATE_IS_HOMED);
	wrmLbl.setEnabled(cs->status().Flags & STATE_WINDING_RES_MISMATCH);
	engLbl.setEnabled(cs->status().Flags & STATE_ENGINE_RESPONSE_ERROR);
	extLbl.setEnabled(cs->status().Flags & STATE_EXTIO_ALARM);

	for (int i=0; i<INDICATOR_COUNT; i++) {
		btns[i].setEnabled(cs->status().GPIOFlags & btn_list[i].flag);
	}

	eeprLbl.setEnabled(cs->status().Flags & STATE_EEPROM_CONNECTED);
	//Getting windings and encoder status from GETS
	encdLbl.setEnabled(true);
	switch (cs->status().EncSts)
	{
		case ENC_STATE_ABSENT	: encdLbl.setPalette(palette_grey); break;
		case ENC_STATE_UNKNOWN	: encdLbl.setPalette(palette_grey); break;
		case ENC_STATE_MALFUNC	: 
			encdLbl.setPalette(palette_red); 

			break;
		case ENC_STATE_REVERS	: 
			encdLbl.setPalette(palette_yellow);
 
			break;
		case ENC_STATE_OK		: encdLbl.setPalette(palette_green); break;
		default					: encdLbl.setPalette(palette_grey); break;
	}
	if (!isSpeedUnitRotation(settingsDlg->motorStgs->feedback.FeedbackType) && cs->status().EncSts == ENC_STATE_OK && encoder_advertised == false) {
		Log("Encoder is detected to be working correctly. You may want to use encoder feedback for closed loop control. You may turn it on in Settings -> Device configuration -> [Motor type: DC or stepper] -> Feedback -> Encoder", SOURCE_XILAB, LOGLEVEL_INFO);
		encoder_advertised = true;
	} else if (!isSpeedUnitRotation(settingsDlg->motorStgs->feedback.FeedbackType) && cs->status().EncSts == ENC_STATE_REVERS && encoder_advertised == false) {
		Log("Encoder is detected and works in the reverse direction. You may want to use encoder feedback for closed loop control. You may turn it on in Settings -> Device configuration -> [Motor type: DC or stepper] -> Feedback -> Encoder", SOURCE_XILAB, LOGLEVEL_INFO);
		encoder_advertised = true;
	} else if (encoder_advertised == true && !(cs->status().EncSts == ENC_STATE_REVERS || cs->status().EncSts == ENC_STATE_OK)) {
		encoder_advertised = false;
	}
	wnd1Lbl.setEnabled(true);
	switch (cs->status().WindSts & WIND_A_STATE_OK)
	{
		case WIND_A_STATE_ABSENT	: wnd1Lbl.setPalette(palette_grey); break;
		case WIND_A_STATE_UNKNOWN	: wnd1Lbl.setPalette(palette_grey); break;
		case WIND_A_STATE_MALFUNC	: wnd1Lbl.setPalette(palette_red); break;
		case WIND_A_STATE_OK		: wnd1Lbl.setPalette(palette_green); break;
		default						: wnd1Lbl.setPalette(palette_grey); break;
	}
	wnd2Lbl.setEnabled(true);
	switch (cs->status().WindSts & WIND_B_STATE_OK)
	{
		case WIND_B_STATE_ABSENT	: wnd2Lbl.setPalette(palette_grey); break;
		case WIND_B_STATE_UNKNOWN	: wnd2Lbl.setPalette(palette_grey); break;
		case WIND_B_STATE_MALFUNC	: wnd2Lbl.setPalette(palette_red); break;
		case WIND_B_STATE_OK		: wnd2Lbl.setPalette(palette_green); break;
		default						: wnd2Lbl.setPalette(palette_grey); break;
	}

	//Feature #317
	if((settingsDlg->motorStgs->engine.EngineFlags & ENGINE_LIMIT_RPM) && ((unsigned int)abs(cs->status().CurSpeed) >= settingsDlg->motorStgs->engine.NomSpeed))
		ui->speedValue->setText("<html><span style=\"text-decoration: overline;\">"+ui->speedValue->text()+"</span></html>");
	if((settingsDlg->motorStgs->engine.EngineFlags & ENGINE_LIMIT_VOLT) && ((unsigned int)abs(cs->status().Upwr) >= settingsDlg->motorStgs->engine.NomVoltage))
		ui->pwrVoltageValue->setText("<html><span style=\"text-decoration: overline;\">"+ui->pwrVoltageValue->text()+"</span></html>");
	if((settingsDlg->motorStgs->engine.EngineFlags & ENGINE_LIMIT_CURR) && ((unsigned int)abs(cs->status().Ipwr) >= settingsDlg->motorStgs->engine.NomCurrent))
		ui->pwrCurrentValue->setText("<html><span style=\"text-decoration: overline;\">"+ui->pwrCurrentValue->text()+"</span></html>");
    
	//Attenuator motion
	AttenuatorMotion();
	
	//Cyclic motion
	if(cyclic)
		CyclicMotion();

	// Homing (for general interface only, no attenuator)
	if (!attenuator->isVisible()) { // a hack, please replace
		if ((cs->status().MvCmdSts == (MVCMD_HOME | MVCMD_RUNNING)) && (cs->connect() == true))
			infoBox.show();
		else
			infoBox.close();
	}


	// Script feature
	if (scriptThread != NULL) {
		bool evaling = scriptThread->qengine->isEvaluating();
		scriptStartBtn->setDisabled(evaling);
		scriptStopBtn->setEnabled(evaling);
		if (!evaling)
			scriptStopBtn->setText("Stop");
		scriptLoadBtn->setDisabled(evaling);
		scriptSaveBtn->setDisabled(evaling);
		codeEdit->setTextInteractionFlags(evaling ? Qt::NoTextInteraction : Qt::TextEditorInteraction);
	}
}

long long MainWindow::getMicroposition(libximc::status_t status) // Returns position in microsteps
{
	return status.CurPosition * settingsDlg->motorStgs->getStepFrac() + status.uCurPosition;
}

void MainWindow::AttenuatorMotion()
{
	calibration_t calb;
	calb.A = settingsDlg->uuStgs->getUnitPerStep();
	calb.MicrostepMode = settingsDlg->motorStgs->engine.MicrostepMode;
	switch(state_atten) {
		case IDLE:
			break;
		case START_CALIBRATE:
			infoBox.show();
			if (attenuator->should_move && attenuator->getCalibrationStatus() == Calibration::OK) { // skip phase 1 (calibration) of the 1+2 combination
				state_atten = WAIT_CALIBRATE;
			} else { // perform phase 1 (calibration) and phase 2 (movement) of the 1+2 combination
				attenuator->setCalibrationStatus(Calibration::IN_PROGRESS);
				//set attenuator default position 
				//Positions for first filters [1 ... 8]
				prev_Pos_y = 0;
				//Positions for second filters [9 ... 16]
				prev_Pos_x = 0;
				//offset of positions
				prev_steps_y = 0;
				posit_steps_x = 0;
				devinterface->command_homezero();
				state_atten = WAIT_CALIBRATE;
			}
			break;
		case WAIT_CALIBRATE:
			if(!((cs->statusCalb().MvCmdSts)&(MVCMD_RUNNING))) {
				attenuator->setCalibrationStatus(Calibration::OK);
				if (!attenuator->should_move){ // skip phase 2 (movement) of the 1+2 combination
					state_atten = WAIT_STOP2;
				}
				else { 
					unsigned int Pos1 = 0;//position of filters in first wheel in range [1...8]
					unsigned int Pos2 = 0;//position of filters in second wheel in range [9...16]
					int step = 0;

					const int of_first_wheel = 1;//offset to shift Pos1 in range [0...7]
					attenuator->get_movePosit(Pos1,Pos2);
					if (attenuator->use_one_wheel){
						//change position for one wheel
						posit_steps_y = ((Pos1-of_first_wheel))%FILTER_COUNT;
						current_position = cs->statusCalb().CurPosition;
						target_position = current_position;
						state_atten = WAIT_STOP1;
					}
					else {	
						int new_x = (Pos2-(FILTER_COUNT+1))%FILTER_COUNT; 
						step = -prev_steps_y+mod_atten_func1(new_x,prev_Pos_x,FILTER_COUNT);
						posit_steps_x = step;
						prev_Pos_x = (Pos2-(FILTER_COUNT+1))%FILTER_COUNT;
						posit_steps_y = ((Pos1-of_first_wheel))%FILTER_COUNT;
						prev_Pos_y = (Pos2-(FILTER_COUNT+1))%FILTER_COUNT; 
						current_position = cs->statusCalb().CurPosition;
						target_position = current_position + posit_steps_x;
						devinterface->command_movr_calb(posit_steps_x, calb);
						state_atten = WAIT_STOP1; // WAIT_MOVE1;	
					}
				}
			}
			break;
		case WAIT_STOP1:
			current_position = cs->statusCalb().CurPosition;
			if(!((cs->statusCalb().MvCmdSts)&(MVCMD_RUNNING)) && ((current_position-target_position)<0.5)){
				int steps_at = - mod_atten_func2(posit_steps_y,prev_Pos_y,FILTER_COUNT);	
				prev_steps_y = steps_at;
				prev_Pos_y = posit_steps_y; 
				current_position = cs->statusCalb().CurPosition;
				target_position = current_position + steps_at;
                devinterface->command_movr_calb(steps_at, calb);
				state_atten = WAIT_MOVE2;
			}
			break;
		case WAIT_MOVE2:
			current_position = cs->statusCalb().CurPosition;
			if(!((cs->statusCalb().MvCmdSts)&(MVCMD_RUNNING)) && ((current_position-target_position)<0.5)){
				state_atten = WAIT_STOP2;
			}
			break;
		case WAIT_STOP2:
			state_atten = IDLE;
			infoBox.close();
			break;
		case MOVE_ERROR:
			state_atten = IDLE;
			attenuator->setCalibrationStatus(Calibration::NO_CALIBRATION);
			attenuator->resetAllBut();
			infoBox.close();
			break;
	}
}

bool MainWindow::isNear(double pos1, double pos2)
{
	if (settingsDlg->uuStgs->enable) {
		return (abs(pos1-pos2) <= settingsDlg->uuStgs->getUnitPerStep());
	} else if (isSpeedUnitRotation(settingsDlg->motorStgs->feedback.FeedbackType)) {
		return (abs(pos1-pos2) <= 1); // 1 count is the minimum observable difference here
	} else {
		return (abs(pos1-pos2) <= 1/(double)(1 << (settingsDlg->motorStgs->engine.MicrostepMode-1)));
	}
}

result_t MainWindow::doMove(double target, bool relative)
{
	calibration_t calb;
	if (settingsDlg->uuStgs->enable) {
		calb.A = settingsDlg->uuStgs->getUnitPerStep();
	} else {
		calb.A = 1;
	}
	calb.MicrostepMode = settingsDlg->motorStgs->engine.MicrostepMode;
	if (relative) {
		return devinterface->command_movr_calb(target, calb);
	} else {
		return devinterface->command_move_calb(target, calb);
	}
}

void MainWindow::CyclicMotion()
{
	const int cyclic_wait = 20; // ms
	switch(settingsDlg->cyclicStgs->GetType())
	{
	case CyclicSettings::UnitType::CYCLICBTB:
		if ((cs->status().GPIOFlags & STATE_LEFT_EDGE) && (last_cyclic_move == CYCLIC_LEFT)) {
			devinterface->command_stop();
			msleep(cyclic_wait); // sleeping in a gui update thread? bad doggy, bad
			devinterface->command_right();
			last_cyclic_move = CYCLIC_RIGHT;
		}
		else if ((cs->status().GPIOFlags & STATE_RIGHT_EDGE) && (last_cyclic_move == CYCLIC_RIGHT)){
			devinterface->command_stop();
			msleep(cyclic_wait); // oh well, it's not a super large delay after all
			devinterface->command_left();
			last_cyclic_move = CYCLIC_LEFT;
		} else { // fixes #4714
			if (last_cyclic_move == CYCLIC_RIGHT)
				devinterface->command_right();
			else
				devinterface->command_left();
		}
		prev_cyclic_state = CyclicSettings::UnitType::CYCLICBTB;
		break;
	case CyclicSettings::UnitType::CYCLICPTP_FINE:
		double left_cyc = settingsDlg->cyclicStgs->GetLeft();
		double right_cyc = settingsDlg->cyclicStgs->GetRight();
		if (settingsDlg->uuStgs->enable) {
			left_cyc *= settingsDlg->uuStgs->getUnitPerStep();
			right_cyc *= settingsDlg->uuStgs->getUnitPerStep();
		}

		if (prev_cyclic_state != CyclicSettings::UnitType::CYCLICPTP_FINE) { // fixes #14197-18
			if (last_cyclic_move == CYCLIC_LEFT) {
				if (doMove(left_cyc) == result_ok) {
					prev_cyclic_state = CyclicSettings::UnitType::CYCLICPTP_FINE;
					break;
				}
			} else {
				if (doMove(right_cyc) == result_ok) {
					prev_cyclic_state = CyclicSettings::UnitType::CYCLICPTP_FINE;
					break;
				}
			}
		}
		if (cs->status().MvCmdSts & MVCMD_RUNNING) {
			break;
		}
		if (last_cyclic_move == CYCLIC_LEFT) {
			if (doMove(right_cyc) == result_ok) {
				last_cyclic_move = CYCLIC_RIGHT;
			}
		} else if (last_cyclic_move == CYCLIC_RIGHT) {
			if (doMove(left_cyc) == result_ok) {
				last_cyclic_move = CYCLIC_LEFT;
			}
		}
		break;
	}
}

void MainWindow::SetLblDivisor(double div)
{
	QString powstr;
	if(div == 10)
		powstr = "";
	else
		powstr.setNum(log10(div));
	
	QString numsize="10", powsize="11";
#if defined(__APPLE__) || defined(__LINUX__)
	numsize = "11";
	powsize = "12";
#endif
#if defined(WIN32) || defined(WIN64)
	numsize = "10";
	powsize = "11";
#endif

	QString display_unit;
	display_unit = settingsDlg->uuStgs->getSuffix(UserUnitSettings::TYPE_COORD, settingsDlg->motorStgs->feedback.FeedbackType);
	ui->divisorLbl->setText("<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body><span style=\"font-size:"+numsize+"pt;\">x10<sup style=\"font-size:"+powsize+"pt;\">"+powstr+"</sup>&nbsp;" + display_unit + "</span></body></html>");
	
	if(div == 1) ui->divisorLbl->setText("");
}

void MainWindow::ClearState(int device_mode)
{
    QFont font;
    QString value(tr("-"));

	if(device_mode == FIRMWARE_MODE)
		ui->positionValue->setText(tr("No device"));
	else if(device_mode == BOOTLOADER_MODE)
	{
		settingsDlg->controllerStgs->LoadControllerSettings();
		ui->positionValue->setText(tr("Bootloader v%1.%2.%3").arg(settingsDlg->controllerStgs->bootloader_version.major).arg(settingsDlg->controllerStgs->bootloader_version.minor).arg(settingsDlg->controllerStgs->bootloader_version.release));
	}

    ui->speedValue->setText(value);
    ui->pwrVoltageValue->setText(value);
    ui->pwrCurrentValue->setText(value);
	ui->usbVoltageValue->setText(value);
    ui->usbCurrentValue->setText(value);
    ui->temperatureValue->setText(value);
    ui->deviceValue->setText(value);
    ui->pwmValue->setText(value);
    ui->pwrValue->setText(value);
    ui->lBdrInd->setPixmap(offPix);
    ui->rBdrInd->setPixmap(offPix);
	ui->cmdBufValue->setText(value);

	slipLbl.setDisabled(true);
	pwhtLbl.setDisabled(true);
	homdLbl.setDisabled(true);
	wrmLbl.setDisabled(true);
	engLbl.setDisabled(true);
	extLbl.setDisabled(true);
	for (int i=0; i<INDICATOR_COUNT; i++) {
		btns[i].setDisabled(true);
	}
	eeprLbl.setDisabled(true);
	wnd1Lbl.setDisabled(true);
	wnd2Lbl.setDisabled(true);
	encdLbl.setDisabled(true);
	errcLbl.setDisabled(true);
	errdLbl.setDisabled(true);
	errvLbl.setDisabled(true);

	ui->movingStateLbl->setDisabled(true);
	ui->targetSpeedLbl->setDisabled(true);
	ui->antiplayLbl->setDisabled(true);

    // Clearing font and palette settings
    font.setBold(false);
    ui->pwrVoltageValue->setFont(font);
    ui->pwrCurrentValue->setFont(font);
	ui->usbVoltageValue->setFont(font);
    ui->usbCurrentValue->setFont(font);
    ui->temperatureValue->setFont(font);
    ui->deviceValue->setFont(font);
    ui->pwrVoltageValue->setPalette(ui->pwrVoltageLbl->palette());
    ui->pwrCurrentValue->setPalette(ui->pwrCurrentLbl->palette());
	ui->usbVoltageValue->setPalette(ui->usbVoltageLbl->palette());
    ui->usbCurrentValue->setPalette(ui->usbCurrentLbl->palette());
    ui->temperatureValue->setPalette(ui->temperatureLbl->palette());
    ui->deviceValue->setPalette(ui->deviceLbl->palette());

	//
	ui->leftBdrBtn->setEnabled(false);
	ui->leftBtn->setEnabled(false);
	ui->stopBtn->setEnabled(false);
	ui->sstpBtn->setEnabled(false);
	ui->rightBdrBtn->setEnabled(false);
	ui->rightBtn->setEnabled(false);
	ui->shiftBtn->setEnabled(false);
	ui->moveBtn->setEnabled(false);
	ui->zeroBtn->setEnabled(false);
	ui->cyclicBtn->setEnabled(false);
	ui->chartBtn->setEnabled(false);
	ui->homeBtn->setEnabled(false);

	moveSpinBox.setEnabled(false);
	shiftSpinBox.setEnabled(false);

	ui->encoderValue->setVisible(false);
	ui->statusBar->showMessage("");

	BackToNormalState = true;

	emit SgnDisableStagePages(true);
}

void MainWindow::NormalState()
{
	ui->movingStateLbl->setEnabled(true);
	ui->targetSpeedLbl->setEnabled(true);
	ui->antiplayLbl->setEnabled(true);

	ui->leftBdrBtn->setEnabled(true);
	ui->leftBtn->setEnabled(true);
	ui->stopBtn->setEnabled(true);
	ui->sstpBtn->setEnabled(true);
	ui->rightBdrBtn->setEnabled(true);
	ui->rightBtn->setEnabled(true);
	ui->shiftBtn->setEnabled(true);
	ui->moveBtn->setEnabled(true);
	ui->zeroBtn->setEnabled(true);
	ui->cyclicBtn->setEnabled(true);
	ui->chartBtn->setEnabled(true);
	ui->homeBtn->setEnabled(true);

	moveSpinBox.setEnabled(true);
	shiftSpinBox.setEnabled(true);
	moveSpinBox.setStepFactor(settingsDlg->motorStgs->getStepFrac());
	shiftSpinBox.setStepFactor(settingsDlg->motorStgs->getStepFrac());

	QApplication::processEvents();
}

void MainWindow::OnSettingsBtnPressed()
{
	try {
		if (settingsDlg->isVisible()) {
			settingsDlg->raise();
			settingsDlg->activateWindow();
			return;
		}
		if(settingsDlg->AllPagesFromDeviceToClassToUi() == true) {
			//восстанавливаем положение и размеры окна
			if (QFile::exists(settingsDlg->MakeConfigFilename())) {
				XSettings settings(settingsDlg->MakeConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
				settingsDlg->FromSettingsToUi(&settings);
			}
			else {
				if (QFile::exists(DefaultConfigFilename())) {
					XSettings settings(DefaultConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
					settingsDlg->FromSettingsToUi(&settings);
				}
				else {
					XSettings settings(DefaultLocalConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
					settingsDlg->FromSettingsToUi(&settings);
				}
			}
		
			if(devinterface->getMode() == BOOTLOADER_MODE)
			{
		
			}

			if (settingsDlg->stageStgs->need_load) {
				settingsDlg->StageSettingsFromDeviceToClassToUi();
				emit SgnDisableStagePages(false);
			}

			settingsDlg->show();
			settingsDlg->raise();
			settingsDlg->activateWindow();
		}
		else Log("Failed to open Settings dialog because function " + init_fail_reason + " failed.", SOURCE_XILAB, LOGLEVEL_ERROR);
	} catch (...) {
		Log("Caught exception in MainWindow.Settings.", SOURCE_XILAB, LOGLEVEL_ERROR);
	}
}

void MainWindow::OnChartBtnPressed()
{
	//chartDlg->enabled = true;
	chartDlg->timer->blockSignals(false);
    chartDlg->showAndRespectAutostart();
	chartDlg->raise();
	chartDlg->activateWindow();
}

void MainWindow::OnCyclicBtnPressed()
{
	move_to_left_bdr = false;
	move_to_right_bdr = false;

	if(cyclic == true) 
		return;

	cyclic = true;
	last_cyclic_move = CYCLIC_LEFT;

	if(settingsDlg->cyclicStgs->GetType() == CyclicSettings::UnitType::CYCLICBTB) {
		devinterface->command_left();
	}
	else if(settingsDlg->cyclicStgs->GetType() == CyclicSettings::UnitType::CYCLICPTP_FINE) {
		double first_target = settingsDlg->cyclicStgs->GetLeft();
		if (settingsDlg->uuStgs->enable) {
			first_target *= settingsDlg->uuStgs->getUnitPerStep();
		}
		doMove(first_target);
	}
	attenuator->setCalibrationStatus(Calibration::NO_CALIBRATION);
}

void MainWindow::OnLeftBdrBtnPressed()
{
	cyclic = false;
	doMove(ui->positionSldr->minValue());
}

void MainWindow::OnLeftBtnPressed()
{
	move_to_left_bdr = false;
	move_to_right_bdr = false;
	cyclic = false;

	devinterface->command_left();
}

void MainWindow::OnStopBtnPressed()
{
	move_to_left_bdr = false;
	move_to_right_bdr = false;
	cyclic = false;

	devinterface->command_stop();
}

void MainWindow::OnSstpBtnPressed()
{
	move_to_left_bdr = false;
	move_to_right_bdr = false;
	cyclic = false;

	if ((cs->status().MvCmdSts & MVCMD_RUNNING) == 0) { // Stop Lv.3 - PWOF
		devinterface->command_power_off();
	} else if (cs->status().MvCmdSts == (MVCMD_SSTP | MVCMD_RUNNING)) { // Stop Lv.2 - STOP
		devinterface->command_stop();
	} else { // Stop Lv.1 - SSTP
		devinterface->command_sstp();
	}
}

void MainWindow::OnRightBtnPressed()
{
	move_to_left_bdr = false;
	move_to_right_bdr = false;
	cyclic = false;

	devinterface->command_right();
}

void MainWindow::OnRightBdrBtnPressed()
{
	cyclic = false;
	doMove(ui->positionSldr->maxValue());
}

void MainWindow::OnMoveBtnPressed()
{
	move_to_left_bdr = false;
	move_to_right_bdr = false;
	cyclic = false;
	doMove(moveSpinBox.value());
}

void MainWindow::OnShiftBtnPressed()
{
	move_to_left_bdr = false;
	move_to_right_bdr = false;
	cyclic = false;
	doMove(shiftSpinBox.value(), true);
}

void MainWindow::OnPIDCalibratorShow()
{
	// stop cyclic when PIDCalibratorwindow showed, see #14197
	cyclic = false;
}

void MainWindow::OnZeroBtnPressed(QString side)
{
	devinterface->command_zero();
	attenuator->setCalibrationStatus(Calibration::NO_CALIBRATION);
}


void MainWindow::OnScriptStartBtnPressed()
{
	codeEdit->setTextInteractionFlags(Qt::NoTextInteraction);
	scriptThread->do_eval = true;
}

void MainWindow::OnScriptStopBtnPressed()
{
	OnScriptStopBtnPressed(false);
}

void MainWindow::OnScriptStopBtnPressed(bool hardstop) {
	if (hardstop) {
		ScriptThread *newst = new ScriptThread(this, devinterface);
		ScriptThread *oldst = scriptThread;
		scriptThread = newst;
		p_scriptThread = newst;
		oldst->terminate(); // will memleak by design
		newst->start(QThread::IdlePriority);
		codeEdit->setTextInteractionFlags(Qt::TextEditorInteraction);
		scriptStopBtn->setText("Stop");
	} else {
		scriptThread->qengine->abortEvaluation();
		if (!scriptThread->qengine->isEvaluating())
			codeEdit->setTextInteractionFlags(Qt::TextEditorInteraction);
		else
			QTimer::singleShot(200, this, SLOT(CheckForceStopCondition()));
	}
}

void MainWindow::CheckForceStopCondition() {
	if (scriptThread->qengine->isEvaluating())
		scriptStopBtn->setText("Force stop");
}

void MainWindow::OnScriptBtnPressed() {
	scriptDlg->show();
	scriptDlg->raise();
	scriptDlg->activateWindow();
}

void MainWindow::OnSaveLogBtnPressed() {
	QString filename;
	QDateTime now = QDateTime::currentDateTime();
	filename = QFileDialog::getSaveFileName(NULL, tr("Save log file"), getDefaultPath()+"/log_"+now.toString("yyyy.MM.dd_hh.mm.ss")+".csv", tr("CSV files (*.csv);;Text files (*.txt);;All files (*.*)"));

	if(filename.isEmpty()) {
		return;
	}

	LoggedFile file(filename);
	file.open(QIODevice::WriteOnly);

	QStringList strList;
	for (int i = 0; i < ui->logEdit->rowCount(); ++i) {
		for (int j =0; j < ui->logEdit->columnCount(); ++j) {
			strList << "\"" + ui->logEdit->item(i, j)->text() + "\",";
		}
		strList.back().chop(1); // Remove last comma
		strList << "\r\n";
	}
	file.write( strList.join("").toUtf8().data() );
	file.close();
}

void MainWindow::OnClearLogBtnPressed() {
	mlog->Clear();
	ui->logEdit->clear();
	ui->logEdit->setRowCount(0);
	InitTable();
}

void MainWindow::OnExitBtnPressed() {
	exiting = true;

	if (inited)
		scriptThread->qengine->abortEvaluation();

	if (cs->connect() == true && devinterface->getMode() == FIRMWARE_MODE) 
	{
		if (cs->status().MoveSts & MOVE_STATE_MOVING)  // if engine is moving (fix for #21757)
		{
			result_t result = devinterface->command_sstp(); // first SSTP to initiate stopping
			if (result != result_ok)
			{
				Log("Error sending soft stop command on exit.", SOURCE_XILAB, LOGLEVEL_WARNING);
				return;
			}
		}
		if (exitBox != NULL)
			exitBox->exec();
	} 
	else 
	{
		close();
	}
}

void MainWindow::OnGoHomeBtnPressed()
{
	cyclic = false;
	devinterface->command_home();
	//attenuator->setCalibrationStatus(Calibration::NO_CALIBRATION);
}

void MainWindow::stopByCancel()
{
	devinterface->command_stop();
	state_atten = MOVE_ERROR;
}

void MainWindow::SetCyclicEnable(bool on)
{
	ui->cyclicBtn->setEnabled(on);
	if(!on){
		ui->cyclicBtn->setToolTip("Set Motor->Fine tuneup on to allow Cyclic mode");
		cyclic = false;
	}
	else
		ui->cyclicBtn->setToolTip("");
}

void MainWindow::LoadPosition()
{
	//Важно, чтобы обновить серийник
	settingsDlg->controllerStgs->LoadControllerSettings();
	QString sn = QString(settingsDlg->controllerStgs->device_info.ManufacturerId) + toStr(settingsDlg->controllerStgs->serialnumber);
	Log("Device found [" + QString(device_name) + "].", SOURCE_XILAB, LOGLEVEL_INFO);
	deviceLost = false;
}

void MainWindow::Log(QString string, QString source, int loglevel)
{
	QDateTime now = QDateTime::currentDateTime();
	emit InsertLineSgn(now, string, source, loglevel, settingsDlg->logStgs);
}

void MainWindow::CommandErrorLog(const char* message, result_t result)
{
	if (inited)
		Log(QString("Function ") + message + QString("() failed, result = ") + toStr((int)result), SOURCE_XILAB, LOGLEVEL_ERROR);
}

void MainWindow::CommandWarningLog(const char* message, result_t result)
{
	if (inited)
		Log(QString("Function ") + message + QString("() failed, result = ") + toStr((int)result), SOURCE_XILAB, LOGLEVEL_WARNING);
}

void MainWindow::ScriptExceptionLog(QString message)
{
	if (inited)
		Log(message, SOURCE_SCRIPT, LOGLEVEL_ERROR);
}

void MainWindow::LoadConfigs(QString filename)
{
	if(!QFile::exists(filename))return;
	XSettings settings(filename, QSettings::IniFormat, QIODevice::ReadOnly);
	XSettings default_stgs(DefaultConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
	settingsDlg->AllPagesFromSettingsToUi(&settings, &default_stgs);
	
	//Window params
	this->FromSettingsToUi(&settings, &default_stgs);
	chartDlg->FromSettingsToUi(&settings, &default_stgs);
	scriptDlg->FromSettingsToUi(&settings, &default_stgs);
	settingsDlg->FromSettingsToUi(&settings, &default_stgs);
	
}
void MainWindow::SaveConfigs(QString filename)
{
	XSettings settings(filename, QSettings::IniFormat, QIODevice::ReadWrite);

	settingsDlg->AllPagesFromUiToSettings(&settings);

	//Window params
	this->FromUiToSettings(&settings);
	chartDlg->FromUiToSettings(&settings);
	scriptDlg->FromUiToSettings(&settings);
	settingsDlg->FromUiToSettings(&settings);

	settings.sync();
}

void MainWindow::SaveProgramConfigs()
{
	QString filename = MakeProgramConfigFilename();
	XSettings settings(filename, QSettings::IniFormat, QIODevice::ReadWrite);
#ifdef SERVICEMODE
	settings.beginGroup("Service_params");
	settings.setValue("Key", ((PageStep3Wgt*)settingsDlg->pageWgtsLst[PageKeyNum])->GetKey());
	settings.endGroup();
#endif

	// "SaveAxisConfig" part for single axis
	QString sn_str = QString("SN_" + toStr(settingsDlg->controllerStgs->serialnumber));
	settings.beginGroup("Multiaxis");
	settings.setValue("last_group", sn_str);
	settings.endGroup();
	settings.beginGroup(sn_str);
	settings.setValue("0", settingsDlg->controllerStgs->serialnumber);
	settings.endGroup();

	//Log settings
	((PageLogSettingsWgt*)settingsDlg->pageWgtsLst[PageLogSettingsNum])->FromUiToSettings(&settings);
}

void MainWindow::LoadProgramConfigs(bool controls_inited)
{
	QString filename = MakeProgramConfigFilename();
	if(!QFile::exists(filename))return;
	XSettings settings(filename, QSettings::IniFormat, QIODevice::ReadWrite);
	XSettings default_settings(DefaultConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
	
#ifdef SERVICEMODE
	if(controls_inited){
		settings.beginGroup("Service_params");
		((PageStep3Wgt*)settingsDlg->pageWgtsLst[PageKeyNum])->SetKey(settings.value("Key", "0").toString());
		settings.endGroup();
	}
#endif
	if (controls_inited) {
		((PageLogSettingsWgt*)settingsDlg->pageWgtsLst[PageLogSettingsNum])->FromSettingsToUi(&settings, &default_settings);
		((PageLogSettingsWgt*)settingsDlg->pageWgtsLst[PageLogSettingsNum])->FromUiToClass();
	}
}

void MainWindow::FromUiToSettings(QSettings *settings)
{
	settings->beginGroup("mainWindow_params");
	if(!this->isMaximized()){
		settings->setValue("position", pos());
		settings->setValue("size", size());
	}
	settings->setValue("fullScreen", this->isMaximized());
	settings->endGroup();
}
void MainWindow::FromSettingsToUi(QSettings *my_settings, QSettings *default_stgs)
{
	QSettings *settings;
	if (my_settings != NULL)
		settings = my_settings;
	else if (default_stgs != NULL)
		settings = default_stgs;
	else
		return;

	int desktop_width = QApplication::desktop()->width();
	int desktop_height = QApplication::desktop()->height();
	QPoint point;
	QSize size;

	settings->beginGroup("mainWindow_params");

	if(settings->value("fullScreen", false).toBool())
		showMaximized();
	else {
		point = settings->value("position", QPoint(0, 0)).toPoint();
		move(point);
		size = settings->value("size", QSize(800, 650)).toSize();
		size.setWidth(min(size.width(), desktop_width-point.x()));
		size.setHeight(min(size.height(), desktop_height-point.y()));
		resize(size);
	}

	settings->endGroup();
}

void MainWindow::slider_borders(double left, double right, double position, double *slider_left, double *slider_right)
{
	double lower = min(left, right);
	double higher = max(left, right);
	bool swapped = (left > right);

	if(!settingsDlg->sliderStgs->watchoverlimits){
		*slider_left = left;
		*slider_right = right;
		return;
	}

	if((position<=higher) && (position >= lower)){
		*slider_left = left;
		*slider_right = right;
		return;
	}
	if(position < lower){
		*slider_left = position;
		*slider_right = higher - (lower - position);
	}
	else if(position > higher){
		*slider_right = position;
		*slider_left = lower + (position - higher);
	}
	if (swapped) {
		std::swap(*slider_right, *slider_left);
	}
}

void MainWindow::keyPressEvent(QKeyEvent* event) 
{
	if (event->isAutoRepeat())
		event->accept();
	switch ( event->key() ) {
		case Qt::Key_Left:
			devinterface->command_left();
			break;
		case Qt::Key_Right:
			devinterface->command_right();
			break;
		default:
			event->ignore();
			break;
	}
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
	switch ( event->key() ) {
		case Qt::Key_Left:
		case Qt::Key_Right:
			devinterface->command_sstp();
			break;
        default:
			event->ignore();
			break;
	}
}

bool MainWindow::eventFilter(QObject *object, QEvent *e)
{
	if (e->type() == QEvent::WindowActivate) {
		if (settingsDlg->isVisible())
			settingsDlg->raise();
		if (chartDlg->isVisible())
			chartDlg->raise();
		if (scriptDlg->isVisible())
			scriptDlg->raise();
		this->raise();
		this->activateWindow();
	}
	return false;
}
