#include <ctime>
#include <chartdlg.h>
#include <chartlog.h>
#include <mainwindow.h>
#include <graphscaleengine.h>
#include <graphscaledraw.h>
#include <functions.h>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QDateTime>
#include <QFileDialog>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <QMessageBox>
#include <loggedfile.h>

ChartDlg::ChartDlg(QWidget *parent, SettingsDlg *settingsDlg, QString title, bool _is_viewer)
    : QDialog(parent)
{
	local_cs = settingsDlg->devinterface->cs;
	local_settingsDlg = settingsDlg;
	local_motorStgs = settingsDlg->motorStgs;
	local_entype = local_motorStgs->entype.EngineType;

	onceshot = false;
	enabled = false;
	device_connected = false;
	slider_enabled = true;				//состояние слайдера: отпущен, график не остановлен
	inited = false;						//InitUI еще не был вызван, update() без инициализации графиков не работает
										//и чтобы в деструкторе не удалять еще не созданные объекты
	break_by_dataUpdate = false;
	is_viewer = _is_viewer;

	dp = 0;
	global_started = false;
	calc_offset = -1.7e+308;
	summ_offset = 0;
	break_cnt = 0;
	last_curr = NULL;
	lastWidth = 0;
	timeoffset = new TimeOffset();
    ui.setupUi(this);

	basetime = QDateTime::currentDateTimeUtc();
    log = new ChartLog(&basetime);

	//создание графиков и расположение на форме
	int data_type;	//тип данных (позиция, скорость, и т.д.)
	//по этому типу Graphpicker определяет тип отображения: с микрошагом или без.
	for(int i=0; i<PLOTSNUM; i++){
		if(local_entype == ENGINE_TYPE_STEP){
			if(i == POSNUM || i == SPEEDNUM)
				data_type = i;
			else
				data_type = -1;
		}
		else
			data_type = -1;

		plots[i] = new GraphPlot(timeoffset, log, data_type);
		ui.verticalLayout_2->addWidget(plots[i]);
	}
	plotmulti = new GraphPlotMulti(timeoffset, log, -2);
	ui.verticalLayout_2->addWidget(plotmulti);

	scrollBar = new QScrollBar(Qt::Horizontal, this);
	scrLayout = new QVBoxLayout();
	scrLayout->setContentsMargins(0, 0, 5, 0);
	scrLayout->addWidget(scrollBar);
	ui.verticalLayout_2->addLayout(scrLayout);

	delay = 5*UPDATE_INTERVAL;
	if (delay > 1000) delay = 1000;
	if (delay < 200) delay = 200;
	
	for(int i=0; i<100000; i++) markers[i] = NULL;

	QObject::connect(this, SIGNAL(finished(int)), this, SLOT(close()));

	// Remove engine voltage/current entirely
	ui.enCurrentChk->setChecked(false);
	ui.enVoltageChk->setChecked(false);
	ui.enCurrentChk->setHidden(true);
	ui.enVoltageChk->setHidden(true);

	plots[POSNUM]->setVisible(ui.positionChk->isChecked());
	plots[SPEEDNUM]->setVisible(ui.speedChk->isChecked());
	plots[ENCODNUM]->setVisible(ui.encodChk->isChecked());
	plots[CURRANUM]->setVisible(ui.currAChk->isChecked());
	plots[CURRBNUM]->setVisible(ui.currBChk->isChecked());
	plots[CURRCNUM]->setVisible(ui.currCChk->isChecked());
	plots[VOLTANUM]->setVisible(ui.voltAChk->isChecked());
	plots[VOLTBNUM]->setVisible(ui.voltBChk->isChecked());
	plots[VOLTCNUM]->setVisible(ui.voltCChk->isChecked());
	plots[ENCURRNUM]->setVisible(ui.enCurrentChk->isChecked());
	plots[ENVOLTNUM]->setVisible(ui.enVoltageChk->isChecked());
	plots[PWRCURRNUM]->setVisible(ui.pwrCurrentChk->isChecked());
	plots[PWRVOLTNUM]->setVisible(ui.pwrVoltageChk->isChecked());
	plots[USBCURRNUM]->setVisible(ui.usbCurrentChk->isChecked());
	plots[USBVOLTNUM]->setVisible(ui.usbVoltageChk->isChecked());
	plots[PWMNUM]->setVisible(ui.pwmChk->isChecked());
	plots[TEMPNUM]->setVisible(ui.tempChk->isChecked());
	plots[JOYNUM]->setVisible(ui.joyChk->isChecked());
	plots[POTNUM]->setVisible(ui.potChk->isChecked());
	plotmulti->setVisible(ui.flagsChk->isChecked());

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->blockSignals(true);

	connect(ui.enCurrentChk,   SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.enVoltageChk,   SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.pwrCurrentChk,  SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.pwrVoltageChk,  SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.usbCurrentChk,  SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.usbVoltageChk,  SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.currAChk,	   SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.currBChk,	   SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.currCChk,	   SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.voltAChk,	   SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.voltBChk,	   SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.voltCChk,	   SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.positionChk,    SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.speedChk,	   SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.encodChk,	   SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.pwmChk,         SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.tempChk,        SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.joyChk,         SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.potChk,         SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));
	connect(ui.flagsChk,       SIGNAL(stateChanged(int)),            this,              SLOT(ChangePlotCnt()));

	connect(ui.startBtn,       SIGNAL(clicked()),			this,              SLOT(OnStartBtnClicked()));
	connect(ui.stopBtn,        SIGNAL(clicked()),			this,              SLOT(OnStopBtnClicked()));
	connect(ui.viewBtn,        SIGNAL(clicked()),			this,              SLOT(OnViewBtnClicked()));
	connect(ui.loadBtn,        SIGNAL(clicked()),			this,              SLOT(OnLoadBtnClicked()));
	connect(ui.saveBtn,        SIGNAL(clicked()),			this,              SLOT(OnSaveBtnClicked()));
	connect(ui.clearBtn,       SIGNAL(clicked()),			this,              SLOT(OnClearBtnClicked()), Qt::QueuedConnection);
	connect(ui.exportBtn,      SIGNAL(clicked()),			this,              SLOT(OnExportBtnClicked()));

	connect(scrollBar,	SIGNAL(valueChanged(int)),			this,			SLOT(sliderUpdate(int)));
	connect(scrollBar,	SIGNAL(sliderPressed()),			this,			SLOT(sliderPressed()));
	connect(scrollBar,	SIGNAL(sliderReleased()),			this,			SLOT(sliderReleased()));

	statusBar = new QStatusBar(this);
	ui.mainVLayout->addWidget(statusBar);
	statusBar->resize(statusBar->size().width(), 20);
	ChangePlotCnt();

	QString wndTitle = "Chart " + title;
	setWindowTitle(wndTitle);

	flag_list	<< STATE_RIGHT_EDGE
				<< STATE_LEFT_EDGE
				<< STATE_BUTTON_RIGHT
				<< STATE_BUTTON_LEFT
				<< STATE_GPIO_LEVEL
				<< STATE_BRAKE
				<< STATE_REV_SENSOR
				<< STATE_SYNC_INPUT
				<< STATE_SYNC_OUTPUT
				;

	csvHeader
		<< "Encoder position"
		<< "Step position"
    	<< "Speed"
		<< "Engine current"
		<< "Engine voltage"
		<< "Power current"
		<< "Power voltage"
		<< "USB current"
		<< "USB voltage"
    	<< "Current A"
		<< "Current B"
		<< "Current C"
    	<< "Voltage A"
		<< "Voltage B"
		<< "Voltage C"
		<< "PWM level"
		<< "Temperature"
		<< "Joystick"
		<< "Analog input"
		<< "Time"
		<< "General flags"
		<< "GPIO flags"
		;

	disableDeprecatedBoxes(local_settingsDlg->controllerStgs->treatAs8SMC5());
 }

ChartDlg::~ChartDlg()
{
	if(inited){
		speedLimitMarker[0].detach();
		speedLimitMarker[1].detach();
		voltLimitMarker[0].detach();
		voltLimitMarker[1].detach();
		currLimitMarker.detach();
		for (int i=0; i<PLOTSNUM; i++) delete plots[i];
	}
	delete plotmulti;
    delete log;
}

void ChartDlg::showAndRespectAutostart() {
	bool was_visible = isVisible();
	this->show();
	if (gStgs->commonStgs->autostart && !was_visible)
		ui.startBtn->click();
}


void ChartDlg::InitUI(GraphSettings *graphStgs)
{
	//init GUI elements
	statusBar->setSizeGripEnabled(false);

	fileopenLbl.setText(" ");
	fileopenLbl.setAlignment(Qt::AlignLeft);
	statusBar->addPermanentWidget(&fileopenLbl);
	fileopenLbl.setVisible(false);

	filesizeLbl.setText(" data size: 0 KB ");
	filesizeLbl.setAlignment(Qt::AlignLeft);
	statusBar->addPermanentWidget(&filesizeLbl);

	stateLbl.setText(" stopped ");
	stateLbl.setFixedWidth(65);
	stateLbl.setAlignment(Qt::AlignCenter);
	statusBar->addPermanentWidget(&stateLbl);

	gStgs = graphStgs;

	datapoints = gStgs->commonStgs->datapoints*(1000/UPDATE_INTERVAL);

	scrollBar->setMinimum(0);
	scrollBar->setMaximum(0);
	scrollBar->setValue(0);
	scrollBar->setPageStep(datapoints);
	
	//при изменении пределов шкалы пересчитывается множитель
	QSignalMapper *signalMapper = new QSignalMapper(this);
	connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(updateDivisor(int)));
	for (int i = 0; i < PLOTSNUM; i++) {
		signalMapper->setMapping((QObject*)plots[i]->axisWidget(QwtPlot::yLeft), i);
		connect((QObject*)plots[i]->axisWidget(QwtPlot::yLeft), SIGNAL(scaleDivChanged()), signalMapper, SLOT(map()));
	}

	CommonPlotSetup();
	timer->start(gStgs->commonStgs->interval);
	ClearPlots(true);

	//маркеры для обозначения ограничителей
	QPen pen(QBrush(QColor(Qt::red)), 1, Qt::DashLine);
	for(int i=0; i<2; i++){
		speedLimitMarker[i].setLineStyle(QwtPlotMarker::HLine);
		speedLimitMarker[i].setZ(1000);
		speedLimitMarker[i].setLinePen(pen);
		speedLimitMarker[i].attach(plots[SPEEDNUM]);
		speedLimitMarker[i].setVisible(local_motorStgs->engine.EngineFlags & ENGINE_LIMIT_RPM);

		voltLimitMarker[i].setLineStyle(QwtPlotMarker::HLine);
		voltLimitMarker[i].setZ(1000);
		voltLimitMarker[i].setLinePen(pen);
		voltLimitMarker[i].attach(plots[ENVOLTNUM]);
		voltLimitMarker[i].setVisible(local_motorStgs->engine.EngineFlags & ENGINE_LIMIT_VOLT);
	}
	speedLimitMarker[0].setYValue(local_motorStgs->engine.NomSpeed);
	speedLimitMarker[1].setYValue(-(double)local_motorStgs->engine.NomSpeed);
	voltLimitMarker[0].setYValue(local_motorStgs->engine.NomVoltage/100.0);
	voltLimitMarker[1].setYValue(-(double)local_motorStgs->engine.NomVoltage/100.0);

	currLimitMarker.setLineStyle(QwtPlotMarker::HLine);
	currLimitMarker.setYValue(local_motorStgs->engine.NomCurrent);
	currLimitMarker.setZ(1000);
	currLimitMarker.setLinePen(pen);
	currLimitMarker.attach(plots[ENCURRNUM]);
	currLimitMarker.setVisible(local_motorStgs->engine.EngineFlags & ENGINE_LIMIT_CURR);

	ui.clearBtn->setVisible(!is_viewer);
	ui.startBtn->setVisible(!is_viewer);
	ui.stopBtn->setVisible(!is_viewer);
	ui.saveBtn->setVisible(!is_viewer);
	ui.viewBtn->setVisible(!is_viewer);
	ui.exportBtn->setVisible(!is_viewer);
	ui.loadBtn->setVisible(is_viewer); // the only button that is available in viewer

	inited = true;
}

void ChartDlg::updateSettings()
{
	//обновление параметров маркеров-ограничителей
	for(int i=0; i<2; i++){
		speedLimitMarker[0].setVisible(local_motorStgs->engine.EngineFlags & ENGINE_LIMIT_RPM);
		voltLimitMarker[i].setVisible(local_motorStgs->engine.EngineFlags & ENGINE_LIMIT_VOLT);
	}
	speedLimitMarker[0].setYValue(local_motorStgs->engine.NomSpeed);
	speedLimitMarker[1].setYValue(-(double)local_motorStgs->engine.NomSpeed);
	voltLimitMarker[0].setYValue(local_motorStgs->engine.NomVoltage/100.0);
	voltLimitMarker[1].setYValue(-(double)local_motorStgs->engine.NomVoltage/100.0);

	currLimitMarker.setYValue(local_motorStgs->engine.NomCurrent);
	currLimitMarker.setVisible(local_motorStgs->engine.EngineFlags & ENGINE_LIMIT_CURR);

	QString new_position_unit = local_settingsDlg->uuStgs->getSuffix(UserUnitSettings::TYPE_COORD, local_settingsDlg->motorStgs->feedback.FeedbackType);
	QString new_speed_unit = local_settingsDlg->uuStgs->getSuffix(UserUnitSettings::TYPE_SPEED, local_settingsDlg->motorStgs->feedback.FeedbackType);
	double new_uu_ratio = local_settingsDlg->uuStgs->getUnitPerStep();
	bool new_uu_enabled = local_settingsDlg->uuStgs->enable;
	if (position_unit != new_position_unit || speed_unit != new_speed_unit || uu_ratio != new_uu_ratio || uu_enabled != new_uu_enabled) {
		position_unit = new_position_unit;
		speed_unit = new_speed_unit;
		uu_ratio = new_uu_ratio;
		uu_enabled = new_uu_enabled;
		plots[POSNUM]->setCaptions(position_unit, "Position");
		plots[SPEEDNUM]->setCaptions(speed_unit, "Speed");

		// Erase old position and speed measurements because they are invalid now
		log->cleanPositionSpeed();
	}

	update();
}

void ChartDlg::InitMarker(QwtPlotMarker *mark, double x, double size)
{
	QPen pen;
	if(size < 10) size = 0.5;
	else if (size < 100) size = 1.0;
	else if (size < 1000) size = 1.5;
	else size = 2.0;

	pen.setColor(QColor(100,100,100));
	pen.setWidthF(size);
	mark->setLinePen(pen);
	mark->setLineStyle(QwtPlotMarker::VLine);
	mark->setXValue(x);
}

void ChartDlg::plotsReplot()
{
	if(ui.currAChk->isChecked())		plots[CURRANUM]->replot();
	if(ui.currBChk->isChecked())		plots[CURRBNUM]->replot();
	if(ui.currCChk->isChecked())		plots[CURRCNUM]->replot();
	if(ui.voltAChk->isChecked())		plots[VOLTANUM]->replot();
	if(ui.voltBChk->isChecked())		plots[VOLTBNUM]->replot();
	if(ui.voltCChk->isChecked())		plots[VOLTCNUM]->replot();
	if(ui.enVoltageChk->isChecked())	plots[ENVOLTNUM]->replot();
	if(ui.pwrVoltageChk->isChecked())	plots[PWRVOLTNUM]->replot();
	if(ui.usbVoltageChk->isChecked())	plots[USBVOLTNUM]->replot();
	if(ui.positionChk->isChecked())		plots[POSNUM]->replot();
	if(ui.speedChk->isChecked())		plots[SPEEDNUM]->replot();
	if(ui.encodChk->isChecked())		plots[ENCODNUM]->replot();
	if(ui.enCurrentChk->isChecked())	plots[ENCURRNUM]->replot();
	if(ui.pwrCurrentChk->isChecked())	plots[PWRCURRNUM]->replot();
	if(ui.usbCurrentChk->isChecked())	plots[USBCURRNUM]->replot();
	if(ui.pwmChk->isChecked())			plots[PWMNUM]->replot();
	if(ui.tempChk->isChecked())			plots[TEMPNUM]->replot();
	if(ui.joyChk->isChecked())			plots[JOYNUM]->replot();
	if(ui.potChk->isChecked())			plots[POTNUM]->replot();
	if(ui.flagsChk->isChecked())		plotmulti->replot();
}

void ChartDlg::CommonPlotSetup()
{
	position_unit = local_settingsDlg->uuStgs->getSuffix(UserUnitSettings::TYPE_COORD, local_settingsDlg->motorStgs->feedback.FeedbackType);
	speed_unit = local_settingsDlg->uuStgs->getSuffix(UserUnitSettings::TYPE_SPEED, local_settingsDlg->motorStgs->feedback.FeedbackType);

	plots[POSNUM]->Init(plots, gStgs->positionStgs, position_unit, "Position");
	plots[SPEEDNUM]->Init(plots, gStgs->speedStgs, speed_unit, "Speed");
	plots[ENCODNUM]->Init(plots, gStgs->encoderStgs, "1", "Encoder");
	plots[ENVOLTNUM]->Init(plots, gStgs->envoltageStgs, "V", "Engine voltage");
	plots[PWRVOLTNUM]->Init(plots, gStgs->pwrvoltageStgs, "V", "Power voltage");
	plots[USBVOLTNUM]->Init(plots, gStgs->usbvoltageStgs, "V", "USB voltage");
	plots[VOLTANUM]->Init(plots, gStgs->voltAStgs, "V", "Voltage A");
	plots[VOLTBNUM]->Init(plots, gStgs->voltBStgs, "V", "Voltage B");
	plots[VOLTCNUM]->Init(plots, gStgs->voltCStgs, "V", "Voltage C");
	plots[CURRANUM]->Init(plots, gStgs->currAStgs, "mA", "Current A");
	plots[CURRBNUM]->Init(plots, gStgs->currBStgs, "mA", "Current B");
	plots[CURRCNUM]->Init(plots, gStgs->currCStgs, "mA", "Current C");
	plots[ENCURRNUM]->Init(plots, gStgs->encurrentStgs, "mA", "Engine current");
	plots[PWRCURRNUM]->Init(plots, gStgs->pwrcurrentStgs, "mA", "Power current");
	plots[USBCURRNUM]->Init(plots, gStgs->usbcurrentStgs, "mA", "USB current");
	plots[PWMNUM]->Init(plots, gStgs->pwmStgs, "%", "PWM level");
	plots[TEMPNUM]->Init(plots, gStgs->temperatureStgs, "<sup>O</sup>C", "Temperature");
	plots[JOYNUM]->Init(plots, gStgs->joyStgs, "1", "Joystick");
	plots[POTNUM]->Init(plots, gStgs->potStgs, "1", "Analog input");
	plotmulti->Init(gStgs->flagsStgs, " ", "Flags");
}

void ChartDlg::updatePlotsRightDist() //todo fix for plotmulti
{
	//ищем максимальный
	int width = lastWidth;

	for(int i=0; i<PLOTSNUM; i++){
		int top_value = plots[i]->axisScaleDraw(QwtPlot::xTop)->maxLabelWidth(plots[i]->axisFont(QwtPlot::xTop));
		int bottom_value = plots[i]->axisScaleDraw(QwtPlot::xBottom)->maxLabelWidth(plots[i]->axisFont(QwtPlot::xBottom));
		if(width < top_value) width = top_value;
		if(width < bottom_value) width = bottom_value;
	}
	int top_value = plotmulti->axisScaleDraw(QwtPlot::xTop)->maxLabelWidth(plotmulti->axisFont(QwtPlot::xTop));
	int bottom_value = plotmulti->axisScaleDraw(QwtPlot::xBottom)->maxLabelWidth(plotmulti->axisFont(QwtPlot::xBottom));
	if(width < top_value) width = top_value;
	if(width < bottom_value) width = bottom_value;

	width/=2;

	if(width == lastWidth) return;

	//не позволяем промежутку уменьшиться до 0
	if(width<3) width = 3;
	for(int i=0; i<PLOTSNUM; i++){
		if(!plots_visible[i]) continue;
		plots[i]->axisScaleDraw(QwtPlot::yRight)->setMinimumExtent(width);
		#if defined(__LINUX__) || defined(__APPLE__)
		plots[i]->enableAxis(QwtPlot::yRight, false);
		plots[i]->enableAxis(QwtPlot::yRight, true);
		#endif
		plots[i]->updateLayout();
	}
	if (plots_visible[PLOTSNUM]) {
		plotmulti->axisScaleDraw(QwtPlot::yRight)->setMinimumExtent(width);
		#if defined(__LINUX__) || defined(__APPLE__)
		plotmulti->enableAxis(QwtPlot::yRight, false);
		plotmulti->enableAxis(QwtPlot::yRight, true);
		#endif
		plotmulti->updateLayout();
	}
	
	lastWidth = width;
}

//вызывается при изменении настроек
void ChartDlg::ReinitPlots()
{
	if(timer->interval() != gStgs->commonStgs->interval)
		timer->setInterval(gStgs->commonStgs->interval);

	datapoints = gStgs->commonStgs->datapoints*(1000/UPDATE_INTERVAL);

	plots[POSNUM]->ReInit(gStgs->positionStgs);
	plots[SPEEDNUM]->ReInit(gStgs->speedStgs);
	plots[ENCODNUM]->ReInit(gStgs->encoderStgs);
	plots[ENVOLTNUM]->ReInit(gStgs->envoltageStgs);
	plots[PWRVOLTNUM]->ReInit(gStgs->pwrvoltageStgs);
	plots[USBVOLTNUM]->ReInit(gStgs->usbvoltageStgs);
	plots[VOLTANUM]->ReInit(gStgs->voltAStgs);
	plots[VOLTBNUM]->ReInit(gStgs->voltBStgs);
	plots[VOLTCNUM]->ReInit(gStgs->voltCStgs);
	plots[ENCURRNUM]->ReInit(gStgs->encurrentStgs);
	plots[PWRCURRNUM]->ReInit(gStgs->pwrcurrentStgs);
	plots[USBCURRNUM]->ReInit(gStgs->usbcurrentStgs);
	plots[CURRANUM]->ReInit(gStgs->currAStgs);
	plots[CURRBNUM]->ReInit(gStgs->currBStgs);
	plots[CURRCNUM]->ReInit(gStgs->currCStgs);
	plots[PWMNUM]->ReInit(gStgs->pwmStgs);
	plots[TEMPNUM]->ReInit(gStgs->temperatureStgs);
	plots[JOYNUM]->ReInit(gStgs->joyStgs);
	plots[POTNUM]->ReInit(gStgs->potStgs);
	plotmulti->ReInit(gStgs->flagsStgs);

	//для перерисовки выключенных графиков 
	bool enabled_temp = enabled;
	enabled = true;
	update();
	enabled = enabled_temp;
	plotsReplot();
}

void ChartDlg::close()
{
	timer->blockSignals(true);
	if (!is_viewer) {
		enabled = false;
		local_cs->setChartsEnabled(false);
	}
}

void ChartDlg::OnStartBtnClicked()
{
	local_cs->setChartsEnabled(true);
	enabled = true;
	statusBar->showMessage("Graphics started", MESSAGE_TIMEOUT);
}

void ChartDlg::OnStopBtnClicked()
{
	enabled = false;
	local_cs->setChartsEnabled(false);
	statusBar->showMessage("Graphics stopped", MESSAGE_TIMEOUT);
}

void ChartDlg::InsertMeasure()
{
	bool connect = local_cs->connect();
	if(!connect)
		return;

	QDateTime msrTime = local_cs->lastUpdateTime();
	measurement_t meas;
	meas.status = local_cs->status();
	meas.status_calb = local_cs->statusCalb();
	meas.chart_data = local_cs->chartData();
	meas.analog_data = local_cs->analogData();
	meas.enabled_calb = local_cs->useCalibration();
	meas.step_factor = local_motorStgs->getStepFrac();
	meas.fbs = local_motorStgs->feedback;

	//одиночное измерение
	if(onceshot){
		log->InsertMeasure(0, msrTime, meas);
		setFileChanged();
		onceshot = false;
	}

	//останов графиков при выключенном моторе
	break_by_dataUpdate = gStgs->commonStgs->dataUpdate && !(meas.status.MoveSts & MOVE_STATE_MOVING);
	//если при включенных графиках произошел дисконнект - останавливаем графики
	device_connected = connect;

	//вызывается только один раз, в момент первой отрисовки старта
	if(!global_started && !break_by_dataUpdate && enabled){
		global_started = true;
		basetime = QDateTime::currentDateTimeUtc();
		dp = 0;
		common_offset = 0;
	}

	if(global_started && enabled && slider_enabled && !break_by_dataUpdate){
		log->InsertMeasure(0, msrTime, meas);
		setFileChanged();
		stateLbl.setText(" run ");
	}
	else
		stateLbl.setText(" stopped ");
}

void ChartDlg::normMeasures()
{
	TList *curr = log->list;
	
	//выравнивание
	if(last_curr == NULL)
		while((curr != NULL) && (curr->next != NULL) && (curr->next->next != NULL)) curr = curr->next;
	else
		curr = last_curr;

	double curr_msec;
	//double prev_msec = 0;
	while((curr != NULL) && (curr->next != NULL))
	{
		if((curr->msec - curr->next->msec) > delay){
			curr_msec = curr->msec;
			curr->msec = curr->next->msec + UPDATE_INTERVAL;

			if(curr_msec > calc_offset){
				setBreakLine(curr->next->msec, curr_msec - common_offset);
				timeoffset->addOffset(curr->next->msec, curr_msec - common_offset);
				common_offset += curr_msec - curr->next->msec - UPDATE_INTERVAL - common_offset;
			}
			calc_offset = curr_msec + delay;
		}
		curr = curr->back;
	}
}

void ChartDlg::update(int int_offset)
{
	if(log->size == 0) return;
	if(!enabled && (int_offset<0)) return;
	if(!inited || (break_by_dataUpdate && (int_offset<0))) return;
	if(!device_connected && (int_offset<0)) return;

	normMeasures();

	TList *curr = log->list;
	if(log->list != NULL) last_curr = log->list->next;

	if(int_offset >= 0){
		curr = log->list;
		int i=0;
		while((i<int_offset) && (curr!=NULL))
		{
			curr = curr->next;
			i++;
		}
	}
	else{
		if(!enabled)return;
		int scrollMax = 0;
		if(log->size > datapoints) scrollMax = log->size - datapoints;
		scrollBar->blockSignals(true);
		scrollBar->setMaximum(scrollMax);
		scrollBar->setValue(scrollBar->maximum());
		scrollBar->blockSignals(false);

		curr = log->list;
	}

	counter = 0;
	double last_succes_timeData = 0;
	for (int i=0; i<PLOTSNUM; i++) {
		data[i].clear();
		data[i].reserve(datapoints);
	}
	for (int i=0; i<DIGNUM; i++) {
		data2[i].clear();
		data2[i].reserve(datapoints);
	}
	data_gpioflags.clear();
	data_gpioflags.reserve(datapoints);
	timeData.clear();
	timeData.reserve(datapoints);

	for(int i = 0; i<datapoints; i++){
		if(curr != NULL){
			if(ui.encodChk->isChecked())		data[ENCODNUM].push_back(curr->encoder_position);
			if(ui.enVoltageChk->isChecked())	data[ENVOLTNUM].push_back(curr->engineVoltage);
			if(ui.pwrVoltageChk->isChecked())	data[PWRVOLTNUM].push_back(curr->pwrVoltage);
			if(ui.usbVoltageChk->isChecked())	data[USBVOLTNUM].push_back(curr->usbVoltage);
			if(ui.voltAChk->isChecked())		data[VOLTANUM].push_back(curr->voltageA);
			if(ui.voltBChk->isChecked())		data[VOLTBNUM].push_back(curr->voltageB);
			if(ui.voltCChk->isChecked())		data[VOLTCNUM].push_back(curr->voltageC);
			if(ui.currAChk->isChecked())		data[CURRANUM].push_back(curr->currentA);
			if(ui.currBChk->isChecked())		data[CURRBNUM].push_back(curr->currentB);
			if(ui.currCChk->isChecked())		data[CURRCNUM].push_back(curr->currentC);
			if(ui.speedChk->isChecked())		data[SPEEDNUM].push_back(curr->speed);
			if(ui.enCurrentChk->isChecked())	data[ENCURRNUM].push_back(curr->engineCurrent);
			if(ui.pwrCurrentChk->isChecked())	data[PWRCURRNUM].push_back(curr->pwrCurrent);
			if(ui.usbCurrentChk->isChecked())	data[USBCURRNUM].push_back(curr->usbCurrent);
			if(ui.pwmChk->isChecked())			data[PWMNUM].push_back(curr->pwm);
			if(ui.tempChk->isChecked())			data[TEMPNUM].push_back(curr->temp);
			if(ui.joyChk->isChecked())			data[JOYNUM].push_back(curr->joy);
			if(ui.potChk->isChecked())			data[POTNUM].push_back(curr->pot);
			if(ui.positionChk->isChecked())		data[POSNUM].push_back(curr->position);
			if(ui.flagsChk->isChecked()) {
				data_genrflags.push_back(curr->GENRflags);
				data_gpioflags.push_back(curr->GPIOflags);
				for (int i=0;i<DIGNUM;i++) {
					if (!(curr->GPIOflags & flag_list.at(i)))
						data2[i].push_back(i+1.0);
					else
						data2[i].push_back(i+1.9);
				}
			}

			timeData.push_back(curr->msec);
			last_succes_timeData = curr->msec;
			curr = curr->next;
		}else{

			for(int j=0; j<PLOTSNUM; j++)
				data[j].push_back(0);
			data_genrflags.push_back(0);
			data_gpioflags.push_back(0);
			for (int i=0;i<DIGNUM;i++) {
				data2[i].push_back(i+1.0);
			}
	
			timeData.push_back(last_succes_timeData - counter*UPDATE_INTERVAL);
			counter++;
		}
	}

	dp = min(log->size, datapoints);

	for(int i=0; i<PLOTSNUM; i++){
		if(plots_visible[i]) {
			plots[i]->setAxisScale(QwtPlot::xTop, timeData.at(0) - gStgs->commonStgs->datapoints*1000, timeData.at(0));
			plots[i]->setAxisScale(QwtPlot::xBottom, timeData.at(0) - gStgs->commonStgs->datapoints*1000, timeData.at(0));

			plots[i]->getCurve()->setSamples(&timeData[0], &(data[i].at(0)), dp-1);
		}
	}
	if (plots_visible[PLOTSNUM]) {
		plotmulti->setAxisScale(QwtPlot::xTop, timeData.at(0) - gStgs->commonStgs->datapoints*1000, timeData.at(0));
		plotmulti->setAxisScale(QwtPlot::xBottom, timeData.at(0) - gStgs->commonStgs->datapoints*1000, timeData.at(0));
		for (int i=0; i<DIGNUM; i++) {
			plotmulti->setCookedData(&timeData[0], &(data2[i].at(0)), dp-1, i);
		}
	}

	updatePlotsRightDist();
	AlignExtents();
	plotsReplot();

	qint64 size_temp = log->size*log->getDataSize();
	filesizeLbl.setText(QString(tr(" data size: %1 KB ")).arg(size_temp/1000));
}

void ChartDlg::setScaleDrawMinTime(double time)
{
	for(int i=0; i<PLOTSNUM; i++){
		static_cast<TimeScaleDraw *>(plots[i]->axisScaleDraw(QwtPlot::xTop))->setMinTime(time);
		static_cast<TimeScaleDraw *>(plots[i]->axisScaleDraw(QwtPlot::xBottom))->setMinTime(time);
	}
	static_cast<TimeScaleDraw *>(plotmulti->axisScaleDraw(QwtPlot::xTop))->setMinTime(time);
	static_cast<TimeScaleDraw *>(plotmulti->axisScaleDraw(QwtPlot::xBottom))->setMinTime(time);
}

void ChartDlg::updateDivisor(int plotnum)
{
	if (plotnum >= 0 && plotnum < PLOTSNUM)
		updateDivisor(plots[plotnum]);
}

void ChartDlg::updateDivisor(GraphPlot *plot)
{
	double lower = plot->axisScaleDiv(QwtPlot::yLeft)->lowerBound();
	double upper = plot->axisScaleDiv(QwtPlot::yLeft)->upperBound();

	if((lower<-1000000000) || (upper>1000000000))
		plot->setDivisor(10000000);
	else if((lower<-100000000) || (upper>100000000))
		plot->setDivisor(1000000);
	else if((lower<-10000000) || (upper>10000000))
		plot->setDivisor(100000);
	else if((lower<-1000000) || (upper>1000000))
		plot->setDivisor(10000);
	else if((lower<-100000) || (upper>100000))
		plot->setDivisor(1000);
	else if((lower<-10000) || (upper>10000))
		plot->setDivisor(100);
	else if((lower<-1000) || (upper>1000))
		plot->setDivisor(10);
	else
		plot->setDivisor(1);
}

void ChartDlg::updatePlotsVisible()
{
	plots_visible[POSNUM] = ui.positionChk->isChecked();
	plots_visible[SPEEDNUM] = ui.speedChk->isChecked();
	plots_visible[ENCODNUM] = ui.encodChk->isChecked();
	plots_visible[ENVOLTNUM] = ui.enVoltageChk->isChecked();
	plots_visible[ENCURRNUM] = ui.enCurrentChk->isChecked();
	plots_visible[PWRVOLTNUM] = ui.pwrVoltageChk->isChecked();
	plots_visible[PWRCURRNUM] = ui.pwrCurrentChk->isChecked();
	plots_visible[USBVOLTNUM] = ui.usbVoltageChk->isChecked();
	plots_visible[USBCURRNUM] = ui.usbCurrentChk->isChecked();
	plots_visible[CURRANUM] = ui.currAChk->isChecked();
	plots_visible[CURRBNUM] = ui.currBChk->isChecked();
	plots_visible[CURRCNUM] = ui.currCChk->isChecked();
	plots_visible[VOLTANUM] = ui.voltAChk->isChecked();
	plots_visible[VOLTBNUM] = ui.voltBChk->isChecked();
	plots_visible[VOLTCNUM] = ui.voltCChk->isChecked();
	plots_visible[PWMNUM] = ui.pwmChk->isChecked();
	plots_visible[TEMPNUM] = ui.tempChk->isChecked();
	plots_visible[JOYNUM] = ui.joyChk->isChecked();
	plots_visible[POTNUM] = ui.potChk->isChecked();

	plots_visible[PLOTSNUM] = ui.flagsChk->isChecked();
}

void ChartDlg::sliderUpdate(int value)
{
	update(scrollBar->maximum()-value);
}
void ChartDlg::sliderPressed()
{
	sliderBegin = log->list;
	slider_enabled = false;
	timer->blockSignals(true);
}
void ChartDlg::sliderReleased()
{
	slider_enabled = true;
	timer->blockSignals(false);
}

//расположение оси времени - xTop у первого включенного графика и xBottom у последнего
void ChartDlg::AxisUpdate() // todo fix for multi
{
	//убираем все шкалы
	for(int i=0; i<PLOTSNUM; i++){
		plots[i]->enableAxis(QwtPlot::xTop, false);
		plots[i]->enableAxis(QwtPlot::xBottom, false);
	}
	plotmulti->enableAxis(QwtPlot::xTop, false);
	plotmulti->enableAxis(QwtPlot::xBottom, false);

	updatePlotsVisible();
	//xTop
	int first_axis = -1;
	for(int i=0; i<PLOTSNUM; i++){
		if(plots_visible[i]){
			plots[i]->enableAxis(QwtPlot::xTop, true);
			first_axis = i;
			break;
		}
	}
	
	//xBottom
	int last_axis = -1;
	for(int i=PLOTSNUM-1; i>=0; i--){
		if(plots_visible[i]){
			plots[i]->enableAxis(QwtPlot::xBottom, true);
			last_axis = i;
			break;
		}
	}

	if (plots_visible[PLOTSNUM]) {
		if (first_axis == -1) {
			plotmulti->enableAxis(QwtPlot::xTop, true);
		}
		if (last_axis != -1) {
			plots[last_axis]->enableAxis(QwtPlot::xBottom, false);
		}
		plotmulti->enableAxis(QwtPlot::xBottom, true);
	}

	ClearExtents();
	AlignExtents();
}

void ChartDlg::ClearExtents()
{
	for(int i=0; i<PLOTSNUM; i++) {
		if (plots_visible[i]) {
			plots[i]->axisScaleDraw(QwtPlot::yLeft)->setMinimumExtent(0);
			plots[i]->axisScaleDraw(QwtPlot::yRight)->setMinimumExtent(0);
			plots[i]->updateLayout();
		}
	}
	if (plots_visible[PLOTSNUM]) {
		plotmulti->axisScaleDraw(QwtPlot::yLeft)->setMinimumExtent(0);
		plotmulti->axisScaleDraw(QwtPlot::yRight)->setMinimumExtent(0);
		plotmulti->updateLayout();
	}

}

void ChartDlg::AlignExtents()
{
	int left_extent, right_extent;
	int min_left_extent = INT_MAX, min_right_extent = INT_MAX;
	int max_left_extent = 0, max_right_extent = 0;

	// For all visible graphs compute min/max extent...
	for(int i=0; i<PLOTSNUM; i++) {
		if (plots_visible[i]) {
			left_extent = plots[i]->axisScaleDraw(QwtPlot::yLeft)->extent(plots[i]->axisFont(QwtPlot::yLeft));
			min_left_extent = min(left_extent, min_left_extent);
			max_left_extent = max(left_extent, max_left_extent);

			right_extent = plots[i]->axisScaleDraw(QwtPlot::yRight)->extent(plots[i]->axisFont(QwtPlot::yRight));
			min_right_extent = min(right_extent, min_right_extent);
			max_right_extent = max(right_extent, max_right_extent);
		}
	}
	if (plots_visible[PLOTSNUM]) {
		left_extent = plotmulti->axisScaleDraw(QwtPlot::yLeft)->extent(plotmulti->axisFont(QwtPlot::yLeft));
		min_left_extent = min(left_extent, min_left_extent);
		max_left_extent = max(left_extent, max_left_extent);

		right_extent = plotmulti->axisScaleDraw(QwtPlot::yRight)->extent(plotmulti->axisFont(QwtPlot::yRight));
		min_right_extent = min(right_extent, min_right_extent);
		max_right_extent = max(right_extent, max_right_extent);
	}
	
	// ...and if they are different...
	if (min_left_extent == max_left_extent && min_right_extent == max_right_extent) {
		return;
	}

	// ...then apply it to everyone to even them out
	for(int i=0; i<PLOTSNUM; i++) {
		plots[i]->axisScaleDraw(QwtPlot::yLeft)->setMinimumExtent(max_left_extent);
		plots[i]->axisScaleDraw(QwtPlot::yRight)->setMinimumExtent(max_right_extent);
		plots[i]->updateLayout();
	}
	plotmulti->axisScaleDraw(QwtPlot::yLeft)->setMinimumExtent(max_left_extent);
	plotmulti->axisScaleDraw(QwtPlot::yRight)->setMinimumExtent(max_right_extent);
	plotmulti->updateLayout();
}

//действия, выполняемые при включении\выключении графиков
void ChartDlg::ChangePlotCnt()
{
	updatePlotsVisible();

	int cnt=0;
	for(int i=0; i<PLOTSNUM; i++) {
		plots[i]->setVisible(plots_visible[i]);
		if(plots_visible[i])cnt++;
	}
	plotmulti->setVisible(plots_visible[PLOTSNUM]);
	if(plots_visible[PLOTSNUM])cnt++;


	if(cnt == 0){
		scrollBar->hide();
		ui.horizontalSpacer->changeSize(1,20, QSizePolicy::Expanding);
	}
	else{
		ui.horizontalSpacer->changeSize(0,20, QSizePolicy::Fixed);
		scrollBar->show();
	}

	update(0);  // fix for #21347
	AxisUpdate();
	plotsReplot();
}

//загрузка графиков в формате XILab < 1.4.3
//больше не поддерживается

//загрузка графиков в формате 1.4.3 <= XILab < 1.6.3
void ChartDlg::LoadFile_v10(QString filename)
{
	CheckedFile file(filename);
	file.open(QIODevice::ReadOnly);

	TList *curr;

	file.seek(16);

	while(!file.atEnd())
	{
		double upos, uspeed;
		log->size++;
		curr = new TList;
		file.read((char *)&curr->encoder_position,	sizeof(double));
		file.read((char *)&curr->position,			sizeof(double));
		file.read((char *)&upos,					sizeof(double));
		file.read((char *)&curr->speed,				sizeof(double));
		file.read((char *)&uspeed,					sizeof(double));
		file.read((char *)&curr->engineCurrent,		sizeof(double));
		file.read((char *)&curr->engineVoltage,		sizeof(double));
		file.read((char *)&curr->pwrCurrent,		sizeof(double));
		file.read((char *)&curr->pwrVoltage,		sizeof(double));
		file.read((char *)&curr->usbCurrent,		sizeof(double));
		file.read((char *)&curr->usbVoltage,		sizeof(double));
		file.read((char *)&curr->currentA,			sizeof(double));
		file.read((char *)&curr->currentB,			sizeof(double));
		file.read((char *)&curr->currentC,			sizeof(double));
		file.read((char *)&curr->voltageA,			sizeof(double));
		file.read((char *)&curr->voltageB,			sizeof(double));
		file.read((char *)&curr->voltageC,			sizeof(double));
		file.read((char *)&curr->pwm,				sizeof(double));
		file.read((char *)&curr->temp,				sizeof(double));
		file.read((char *)&curr->msec,				sizeof(double));
		file.read((char *)&curr->step_factor,		sizeof(unsigned int));
		file.seek(file.pos() + sizeof(unsigned int)); // skip junk from data alignment

		curr->position += upos / curr->step_factor; // adds former microstep part
		curr->speed += uspeed / curr->step_factor; // adds former microstep part

		//поля данных, отсутствующие в этом формате
		curr->joy = 0;
		curr->pot = 0;

		curr->GENRflags = 0;
		curr->GPIOflags = 0;

		if(log->list == NULL){
			log->list = curr;
			log->list->next = NULL;
			log->list->back = NULL;
		}
		else{
			curr->next = log->list;
			curr->back = NULL;
			log->list->back = curr;
			log->list = curr;
		}
	}

	file.close();
}

//загрузка графиков в формате 1.6.3 <= XILab < 1.8.11
void ChartDlg::LoadFile_v11(QString filename)
{
	CheckedFile file(filename);
	file.open(QIODevice::ReadOnly);

	TList *curr;

	file.seek(16);

	while(!file.atEnd())
	{
		double upos, uspeed;
		log->size++;
		curr = new TList;
		file.read((char *)&curr->encoder_position,	sizeof(double));
		file.read((char *)&curr->position,			sizeof(double));
		file.read((char *)&upos,					sizeof(double));
		file.read((char *)&curr->speed,				sizeof(double));
		file.read((char *)&uspeed,					sizeof(double));
		file.read((char *)&curr->engineCurrent,		sizeof(double));
		file.read((char *)&curr->engineVoltage,		sizeof(double));
		file.read((char *)&curr->pwrCurrent,		sizeof(double));
		file.read((char *)&curr->pwrVoltage,		sizeof(double));
		file.read((char *)&curr->usbCurrent,		sizeof(double));
		file.read((char *)&curr->usbVoltage,		sizeof(double));
		file.read((char *)&curr->currentA,			sizeof(double));
		file.read((char *)&curr->currentB,			sizeof(double));
		file.read((char *)&curr->currentC,			sizeof(double));
		file.read((char *)&curr->voltageA,			sizeof(double));
		file.read((char *)&curr->voltageB,			sizeof(double));
		file.read((char *)&curr->voltageC,			sizeof(double));
		file.read((char *)&curr->pwm,				sizeof(double));
		file.read((char *)&curr->temp,				sizeof(double));
		file.read((char *)&curr->joy,				sizeof(double));
		file.read((char *)&curr->pot,				sizeof(double));
		file.read((char *)&curr->msec,				sizeof(double));
		file.seek(file.pos() + sizeof(unsigned int));// skip flags, because they can't be mapped to GENRflags or GPIOflags alone
		file.read((char *)&curr->step_factor,		sizeof(unsigned int));

		curr->position += upos / curr->step_factor; // adds former microstep part
		curr->speed += uspeed / curr->step_factor; // adds former microstep part

		//поля данных, отсутствующие в этом формате
		curr->GENRflags = 0;
		curr->GPIOflags = 0;

		if(log->list == NULL){
			log->list = curr;
			log->list->next = NULL;
			log->list->back = NULL;
		}
		else{
			curr->next = log->list;
			curr->back = NULL;
			log->list->back = curr;
			log->list = curr;
		}
	}

	file.close();
}

//загрузка графиков в формате 1.8.11 <= XILab < 1.10.7
void ChartDlg::LoadFile_v12(QString filename)
{
	CheckedFile file(filename);
	file.open(QIODevice::ReadOnly);

	TList *curr;

	file.seek(16);

	while(!file.atEnd())
	{
		double upos, uspeed;
		log->size++;
		curr = new TList;
		file.read((char *)&curr->encoder_position,	sizeof(double));
		file.read((char *)&curr->position,			sizeof(double));
		file.read((char *)&upos,					sizeof(double));
		file.read((char *)&curr->speed,				sizeof(double));
		file.read((char *)&uspeed,					sizeof(double));
		file.read((char *)&curr->engineCurrent,		sizeof(double));
		file.read((char *)&curr->engineVoltage,		sizeof(double));
		file.read((char *)&curr->pwrCurrent,		sizeof(double));
		file.read((char *)&curr->pwrVoltage,		sizeof(double));
		file.read((char *)&curr->usbCurrent,		sizeof(double));
		file.read((char *)&curr->usbVoltage,		sizeof(double));
		file.read((char *)&curr->currentA,			sizeof(double));
		file.read((char *)&curr->currentB,			sizeof(double));
		file.read((char *)&curr->currentC,			sizeof(double));
		file.read((char *)&curr->voltageA,			sizeof(double));
		file.read((char *)&curr->voltageB,			sizeof(double));
		file.read((char *)&curr->voltageC,			sizeof(double));
		file.read((char *)&curr->pwm,				sizeof(double));
		file.read((char *)&curr->temp,				sizeof(double));
		file.read((char *)&curr->joy,				sizeof(double));
		file.read((char *)&curr->pot,				sizeof(double));
		file.read((char *)&curr->msec,				sizeof(double));
		file.read((char *)&curr->GENRflags,			sizeof(unsigned int));
		file.read((char *)&curr->GPIOflags,			sizeof(unsigned int));
		file.read((char *)&curr->step_factor,		sizeof(unsigned int));
		file.seek(file.pos() + sizeof(unsigned int)); // skip junk from data alignment
		
		curr->position += upos / curr->step_factor; // adds former microstep part
		curr->speed += uspeed / curr->step_factor; // adds former microstep part

		if(log->list == NULL){
			log->list = curr;
			log->list->next = NULL;
			log->list->back = NULL;
		}
		else{
			curr->next = log->list;
			curr->back = NULL;
			log->list->back = curr;
			log->list = curr;
		}
	}

	file.close();
}

//загрузка графиков в формате 1.10.7 <= XILab < ?
void ChartDlg::LoadFile_v20(QString filename)
{
	CheckedFile file(filename);
	file.open(QIODevice::ReadOnly);

	TList *curr;

	char str[256];
	file.seek(16+1);
	file.read(str, 17);
	plots[POSNUM]->setCaptions(QString::fromUtf8(str), "Position");

	file.seek(16+1+16+1);
	file.read(str, 17);
	plots[SPEEDNUM]->setCaptions(QString::fromUtf8(str), "Speed");

	file.seek(256);

	while(!file.atEnd())
	{
		log->size++;
		curr = new TList;
		file.read((char *)&curr->encoder_position, log->getDataSize());
		file.seek(file.pos() + log->getFillSize());

		if(log->list == NULL){
			log->list = curr;
			log->list->next = NULL;
			log->list->back = NULL;
		}
		else{
			curr->next = log->list;
			curr->back = NULL;
			log->list->back = curr;
			log->list = curr;
		}
	}

	file.close();
}

void ChartDlg::OnViewBtnClicked()
{
	try {
		ChartDlg* shower = new ChartDlg(this, local_settingsDlg, "viewer", true);
		shower->InitUI(local_settingsDlg->graphStgs);
		shower->disableDeprecatedBoxes(false); // allow viewing USB voltage-current
		shower->setWindowModality(Qt::WindowModal);
		QSettings tmp("tmp");
		this->FromUiToSettings(&tmp);
		tmp.sync();
		shower->FromSettingsToUi(&tmp);
		QFile::remove("tmp");
		shower->setAttribute(Qt::WA_DeleteOnClose); // >Makes Qt delete this widget when the widget has accepted the close event
		shower->show();
	} catch (my_exception& e) {
		QMessageBox::warning(0, "An exception has occurred", e.text());
	};
}

void ChartDlg::OnLoadBtnClicked()
{
	try {
		if(default_load_path == "")
			default_load_path = getDefaultPath();
		
		QString filename;

		filename = QFileDialog::getOpenFileName(this, tr("Open graph file"), default_load_path, tr("Graph files (*.grf);;All files (*.*)"));
		
		if(filename.isEmpty()) {
			statusBar->showMessage("Loading cancelled", MESSAGE_TIMEOUT);
			return;
		}

		//обновляем дефолтные пути
		QDir dir;
		dir.setPath(filename);
		dir.cdUp();
		default_load_path = dir.path();

		OnClearBtnClicked();

		fileopenLbl.setText(getFileName(filename)+" ");
		fileopenLbl.setVisible(true);

		LoggedFile file(filename);
		file.open(QIODevice::ReadOnly);
		
		//определение версии файла
		char header_str[16];
		file.read(header_str, 16);
		file.close();

		QString header(header_str);
		if(header.startsWith("XILabGraphs_v2.0"))
			LoadFile_v20(filename);
		else if(header.startsWith("XILabGraphs_v2.")) // version 2 graphs are forward-compatible
			LoadFile_v20(filename); // so use the latest _v2x reading function to load newer unknown 2.x version
		else if(header.startsWith("XILabGraphs_v1.2"))
			LoadFile_v12(filename);
		else if(header.startsWith("XILabGraphs_v1.1"))
			LoadFile_v11(filename);
		else if(header.startsWith("XILabGraphs_v1.0"))
			LoadFile_v10(filename);
		else if(header.startsWith("XILabGraphs_v"))
			throw my_exception("The file has unknown version.");
		else
			throw my_exception("The file doesn't look like a XILab graphs file.");

		if(log->size == 0) return;

		//нормируем измерения: формируем offset, рисуем линии разрыва, пересчитываем значения времени
		normMeasures();

		//пересчет меток времени: смещение всего графика влево на величину длины графика, чтобы вся кривая оказалась < 0 с
		//запоминаем минимальное время графика и передаем его в TimeScaleDraw, чтобы левее этого значения метки не ставились
		int time_offset = log->list->msec + common_offset + UPDATE_INTERVAL;
		MoveGraph(time_offset);
		setScaleDrawMinTime(minTime());

		scrollBar->blockSignals(true);
		int scrollMax = 0;
		if(log->size > datapoints) scrollMax = log->size - datapoints;
		scrollBar->setMaximum(scrollMax);
		scrollBar->setValue(scrollBar->maximum());
		scrollBar->blockSignals(false);	

		update(0);
		updatePlotsRightDist();

		statusBar->showMessage("Graphics loaded", MESSAGE_TIMEOUT);
	} catch (my_exception& e) {
		QMessageBox::warning(0, "An exception has occurred", e.text());
	}
}

double ChartDlg::minTime()
{
	double min_time = 0;
	TList *curr = log->list;
	while (curr != NULL) {
		if(curr->msec < min_time) min_time = curr->msec;
		curr = curr->next;
	}
	return min_time;
}

void ChartDlg::MoveGraph(double time_offset)
{
	//смещаем кривую
	TList *curr = log->list;
	while (curr != NULL) {
		curr->msec -= time_offset;
		curr = curr->next;
	}

	//после смещения графика сбрасываем переменные, отвечающие за разрывы
	calc_offset = 0;


	//смещаем timestamp в offset на time_offset
	timeoffset->moveData(-time_offset);

	//смещаем метки
	for(int i=0; i<10000; i++){
		if(markers[i] == NULL) break;
		else markers[i]->setXValue(markers[i]->xValue()-time_offset);
	}

	//смещаем тики 
	for(int i=0; i<PLOTSNUM; i++){
		static_cast<myScaleEngine *>(plots[i]->axisScaleEngine(QwtPlot::xTop))->move_ticks(-time_offset);
		static_cast<myScaleEngine *>(plots[i]->axisScaleEngine(QwtPlot::xBottom))->move_ticks(-time_offset);
	}
	static_cast<myScaleEngine *>(plotmulti->axisScaleEngine(QwtPlot::xTop))->move_ticks(-time_offset);
	static_cast<myScaleEngine *>(plotmulti->axisScaleEngine(QwtPlot::xBottom))->move_ticks(-time_offset);
}

void ChartDlg::OnSaveBtnClicked()
{
	if(log->size == 0){
		QMessageBox::information(this, tr("Save cancelled"), tr("Charts are empty"));
		return;
	}
	bool enabled_temp = enabled;
	enabled = false;

	if(default_save_path == "")
		default_save_path = getDefaultPath();

	QString filename;
	
	filename = QFileDialog::getSaveFileName(this, tr("Save graph file"), default_save_path, tr("Graph files (*.grf);;All files (*.*)"));
	
	if(filename.isEmpty()) {
		statusBar->showMessage("Saving cancelled", MESSAGE_TIMEOUT);
		enabled = enabled_temp;
		return;
	}

        if(!filename.endsWith(".grf", Qt::CaseInsensitive)){
            filename+=".grf";
        }

	//обновляем дефолтные пути
	QDir dir;
	dir.setPath(filename);
	dir.cdUp();
	default_save_path = dir.path();

	fileopenLbl.setText(getFileName(filename)+" ");
	fileopenLbl.setVisible(true);
	
	TList *curr = log->list;
	LoggedFile file(filename);
	file.open(QIODevice::WriteOnly);

	char zero[256];
	const int header_length = 256;
	memset(&zero[0], 0, 256);
	file.write(&zero[0], header_length);
	file.seek(0);

	//----File header----//
	const char* header = "XILabGraphs_v2.0";
	file.write(header);

	file.seek(16);
	file.write(&zero[0], 1);
	file.write(position_unit.toUtf8().constData()); // QByteArray gives last char as zero, so we can omit string length
	
	file.seek(16+1+16);
	file.write(&zero[0], 1);
	file.write(speed_unit.toUtf8().constData()); // same here

	file.seek(header_length);

	int i=0;
	double time = 0;

	//запись будем вести с конца
	while(curr->next != NULL) curr = curr->next;
	
	while(curr != NULL)
	{
		++i;
		double temp_time = curr->msec;
		curr->msec = timeoffset->CalcValue(temp_time);
		file.write((char *)&curr->encoder_position,	log->getDataSize());
		file.write(&zero[0], log->getFillSize()); // pad every block
		curr->msec = temp_time;
		curr = curr->back;
	}
	
	file.close();

	enabled = enabled_temp;
	statusBar->showMessage("Graphics saved", MESSAGE_TIMEOUT);
}

void ChartDlg::OnClearBtnClicked()
{
	bool enabled_temp = enabled;
	enabled = false;

	timer->blockSignals(true);
	//удаление измерений
	log->delete_list();
	//удаление маркеров
	for(int i=0; i<100000; i++) {
		if(markers[i] != NULL) { markers[i]->detach(); delete markers[i]; markers[i] = NULL; }
	}
	break_cnt = 0;
	last_curr = NULL;
	calc_offset = -1.7e+308;
	common_offset = 0;
	scrollBar->setMaximum(0);
	global_started = false;
	setScaleDrawMinTime(0);
	timeoffset->Clear();
	ClearPlots();
	fileopenLbl.setVisible(false);
	timer->blockSignals(false);
	statusBar->showMessage("Graphics cleared", MESSAGE_TIMEOUT);

	enabled = enabled_temp;
}

inline void write_dbl (QFile &file, double number, unsigned int precision)
{
	file.write( QString::number( number, 'f', precision).append(",").toLocal8Bit().data() );
}
inline void write_uint (QFile &file, unsigned int number)
{
	file.write( QString::number( number ).append(",").toLocal8Bit().data() );
}

void ChartDlg::OnExportBtnClicked()
{
	bool enabled_temp = enabled;
	enabled = false;

	if(default_save_path == "")
		default_save_path = getDefaultPath();

	QString filename;
	
	filename = QFileDialog::getSaveFileName(this, tr("Export CSV file"), default_save_path, tr("CSV files (*.csv);;All files (*.*)"));
	
	if(filename.isEmpty()) {
		statusBar->showMessage("Export cancelled", MESSAGE_TIMEOUT);
		enabled = enabled_temp;
		return;
	}

	if(!filename.endsWith(".csv", Qt::CaseInsensitive)){
		filename+=".csv";
	}

	TList *curr = log->list;
	LoggedFile file(filename);
	file.open(QIODevice::WriteOnly);
	
	//----File header----//
	Q_FOREACH( QString item, csvHeader ) {
		file.write(item.prepend("\"").append("\",").toLocal8Bit().data());
	}
	file.seek(file.pos()-1);
	file.write("\n");

	int i=0;
	double time = 0;

	//запись будем вести с конца
	while(curr->next != NULL) curr = curr->next;
	
	while(curr != NULL)
	{
		++i;
		double temp_time = curr->msec;
		curr->msec = timeoffset->CalcValue(temp_time);

		int j = 0;
		write_dbl(file, curr->encoder_position,	0);
		write_dbl(file, curr->position,			0);
		write_dbl(file, curr->speed,			0);
		write_dbl(file, curr->engineCurrent,	0);
		write_dbl(file, curr->engineVoltage,	2);
		write_dbl(file, curr->pwrCurrent,		0);
		write_dbl(file, curr->pwrVoltage,		2);
		write_dbl(file, curr->usbCurrent,		0);
		write_dbl(file, curr->usbVoltage,		2);
		write_dbl(file, curr->currentA,			0);
		write_dbl(file, curr->currentB,			0);
		write_dbl(file, curr->currentC,			0);
		write_dbl(file, curr->voltageA,			2);
		write_dbl(file, curr->voltageB,			2);
		write_dbl(file, curr->voltageC,			2);
		write_dbl(file, curr->pwm,				3);
		write_dbl(file, curr->temp,				1);
		write_dbl(file, curr->joy,				0);
		write_dbl(file, curr->pot,				0);
		write_dbl(file, curr->msec,				0);
		write_uint(file, curr->GENRflags		 );
		write_uint(file, curr->GPIOflags		 );
		
		file.seek(file.pos()-1); // removes last comma
		file.write("\n");

		curr->msec = temp_time;
		curr = curr->back;
	}
	file.close();

	enabled = enabled_temp;
	statusBar->showMessage("Graphics exported", MESSAGE_TIMEOUT);
}

void ChartDlg::ClearPlots(bool first)
{
	if(first){
		onceshot = true;
		TimeInterval t1;
		while(onceshot && (t1.msecsElapsed()<250)) sleep_act(50);

		for (int i=0; i<PLOTSNUM; i++) {
			data[i].clear();
		}
		for (int i=0; i<DIGNUM; i++) {
			data2[i].clear();
		}
		data_genrflags.clear();
		data_gpioflags.clear();
		timeData.clear();
		if (log->size > 0){
			data[POSNUM].push_back(log->list->position);
			data[ENCODNUM].push_back(log->list->encoder_position);
			data[ENVOLTNUM].push_back(log->list->engineVoltage);
			data[PWRVOLTNUM].push_back(log->list->pwrVoltage);
			data[USBVOLTNUM].push_back(log->list->usbVoltage);
			data[CURRANUM].push_back(log->list->currentA);
			data[CURRBNUM].push_back(log->list->currentB);
			data[CURRCNUM].push_back(log->list->currentC);
			data[VOLTANUM].push_back(log->list->voltageA);
			data[VOLTBNUM].push_back(log->list->voltageB);
			data[VOLTCNUM].push_back(log->list->voltageC);
			data[SPEEDNUM].push_back(log->list->speed);
			data[ENCURRNUM].push_back(log->list->engineCurrent);
			data[PWRCURRNUM].push_back(log->list->pwrCurrent);
			data[USBCURRNUM].push_back(log->list->usbCurrent);
			data[PWMNUM].push_back(log->list->pwm);
			data[TEMPNUM].push_back(log->list->temp);
			data[JOYNUM].push_back(log->list->joy);
			data[POTNUM].push_back(log->list->pot);
			data_genrflags.push_back(log->list->GENRflags);
			data_gpioflags.push_back(log->list->GPIOflags);
		}
		else {
			data[ENVOLTNUM].push_back(0);
			data[ENCODNUM].push_back(0);
			data[PWRVOLTNUM].push_back(0);
			data[USBVOLTNUM].push_back(0);
			data[CURRANUM].push_back(0);
			data[CURRBNUM].push_back(0);
			data[CURRCNUM].push_back(0);
			data[VOLTANUM].push_back(0);
			data[VOLTBNUM].push_back(0);
			data[VOLTCNUM].push_back(0);
			data[POSNUM].push_back(0);
			data[SPEEDNUM].push_back(0);
			data[ENCURRNUM].push_back(0);
			data[PWRCURRNUM].push_back(0);
			data[USBCURRNUM].push_back(0);
			data[PWMNUM].push_back(0);
			data[TEMPNUM].push_back(0);
			data[JOYNUM].push_back(0);
			data[POTNUM].push_back(0);
			data_genrflags.push_back(0);
			data_gpioflags.push_back(0);
		}

		double time[1];	time[0] = 0;

		for(int i=0; i<PLOTSNUM; i++){
			plots[i]->getCurve()->setSamples(time, &data[i][0], 1);
			ClearPlot(plots[i]);
		}
	}
	else{
		double x[1]={0}, y[1]={0};
		for(int i=0; i<PLOTSNUM; i++){
			plots[i]->getCurve()->setSamples(x, y, 0);
			ClearPlot(plots[i]);
		}
		for (int j=0; j<DIGNUM; j++) {
			plotmulti->getCurve(j)->setSamples(x, y, 0);
		}
		ClearPlotMulti(plotmulti);
	}

	log->delete_list();
}

void ChartDlg::ClearPlot(GraphPlot *plot)
{
	static_cast<myScaleEngine *>(plot->axisScaleEngine(QwtPlot::xTop))->delete_ticks();
	static_cast<myScaleEngine *>(plot->axisScaleEngine(QwtPlot::xBottom))->delete_ticks();

	timeData.clear();
	timeData.reserve(datapoints);
	for(int i=0; i<datapoints; i++)
		timeData.push_back(-i*UPDATE_INTERVAL);

	plot->setAxisScale(QwtPlot::xTop, timeData.front(), timeData.back());
	plot->setAxisScale(QwtPlot::xBottom, timeData.front(), timeData.back());
	plot->replot();
}

void ChartDlg::ClearPlotMulti(GraphPlotMulti *plot)
{
	static_cast<myScaleEngine *>(plot->axisScaleEngine(QwtPlot::xTop))->delete_ticks();
	static_cast<myScaleEngine *>(plot->axisScaleEngine(QwtPlot::xBottom))->delete_ticks();

	plot->setAxisScale(QwtPlot::xTop, timeData.front(), timeData.back());
	plot->setAxisScale(QwtPlot::xBottom, timeData.front(), timeData.back());
	plot->replot();
}

void ChartDlg::FromUiToSettings(QSettings *settings)
{
	//положение окна
	settings->beginGroup("graphWindow_params");
	settings->setValue("position", pos());
	settings->setValue("size", size());
    //дефолтные пути
    settings->setValue("load_path", default_load_path);
    settings->setValue("save_path", default_save_path);
	//состояние графиков
	settings->setValue("Position_graph_enable",		ui.positionChk->isChecked());
	settings->setValue("Speed_graph_enable",		ui.speedChk->isChecked());
	settings->setValue("Encoder_graph_enable",		ui.encodChk->isChecked());
	settings->setValue("EnVoltage_graph_enable",	ui.enVoltageChk->isChecked());
	settings->setValue("PwrVoltage_graph_enable",	ui.pwrVoltageChk->isChecked());
	settings->setValue("UsbVoltage_graph_enable",	ui.usbVoltageChk->isChecked());
	settings->setValue("VoltageA_graph_enable",		ui.voltAChk->isChecked());
	settings->setValue("VoltageB_graph_enable",		ui.voltBChk->isChecked());
	settings->setValue("VoltageC_graph_enable",		ui.voltCChk->isChecked());
	settings->setValue("EnCurrent_graph_enable",	ui.enCurrentChk->isChecked());
	settings->setValue("PwrCurrent_graph_enable",	ui.pwrCurrentChk->isChecked());
	settings->setValue("UsbCurrent_graph_enable",	ui.usbCurrentChk->isChecked());
	settings->setValue("CurrentA_graph_enable",		ui.currAChk->isChecked());
	settings->setValue("CurrentB_graph_enable",		ui.currBChk->isChecked());
	settings->setValue("CurrentC_graph_enable",		ui.currCChk->isChecked());
//	settings->setValue("SupVoltage_graph_enable",	ui.pwrVoltageChk->isChecked());
	settings->setValue("PWM_graph_enable",			ui.pwmChk->isChecked());
	settings->setValue("Temperature_graph_enable",	ui.tempChk->isChecked());
	settings->setValue("Joy_graph_enable",			ui.joyChk->isChecked());
	settings->setValue("Pot_graph_enable",			ui.potChk->isChecked());
	settings->setValue("Flags_graph_enable",		ui.flagsChk->isChecked());
	settings->endGroup();
}
void ChartDlg::FromSettingsToUi(QSettings *my_settings, QSettings *default_stgs)
{
	QSettings *settings;
	if (my_settings != NULL)
		settings = my_settings;
	else if (default_stgs != NULL)
		settings = default_stgs;
	else
		return;

	settings->beginGroup("graphWindow_params");

	//состояние графиков
	if(settings->contains("Position_graph_enable"))
		ui.positionChk->setChecked(settings->value("Position_graph_enable", true).toBool());

	if(settings->contains("Speed_graph_enable"))
		ui.speedChk->setChecked(settings->value("Speed_graph_enable", true).toBool());

	if(settings->contains("Encoder_graph_enable"))
		ui.encodChk->setChecked(settings->value("Encoder_graph_enable", true).toBool());

	if(settings->contains("PwrVoltage_graph_enable"))
		ui.pwrVoltageChk->setChecked(settings->value("PwrVoltage_graph_enable", false).toBool());

	if(settings->contains("UsbVoltage_graph_enable"))
		ui.usbVoltageChk->setChecked(settings->value("UsbVoltage_graph_enable", false).toBool());

	if(settings->contains("VoltageA_graph_enable"))
		ui.voltAChk->setChecked(settings->value("VoltageA_graph_enable", false).toBool());

	if(settings->contains("VoltageB_graph_enable"))
		ui.voltBChk->setChecked(settings->value("VoltageB_graph_enable", false).toBool());

	if(settings->contains("VoltageC_graph_enable"))
		ui.voltCChk->setChecked(settings->value("VoltageC_graph_enable", false).toBool());

	if(settings->contains("PwrCurrent_graph_enable"))
		ui.pwrCurrentChk->setChecked(settings->value("PwrCurrent_graph_enable", false).toBool());

	if(settings->contains("UsbCurrent_graph_enable"))
		ui.usbCurrentChk->setChecked(settings->value("UsbCurrent_graph_enable", false).toBool());

	if(settings->contains("CurrentA_graph_enable"))
		ui.currAChk->setChecked(settings->value("CurrentA_graph_enable", false).toBool());

	if(settings->contains("CurrentB_graph_enable"))
		ui.currBChk->setChecked(settings->value("CurrentB_graph_enable", false).toBool());

	if(settings->contains("CurrentС_graph_enable"))
		ui.currCChk->setChecked(settings->value("CurrentС_graph_enable", false).toBool());

	if(settings->contains("PWM_graph_enable"))
		ui.pwmChk->setChecked(settings->value("PWM_graph_enable", false).toBool());

	if(settings->contains("Temperature_graph_enable"))
		ui.tempChk->setChecked(settings->value("Temperature_graph_enable", false).toBool());

	if(settings->contains("Joy_graph_enable"))
		ui.joyChk->setChecked(settings->value("Joy_graph_enable", false).toBool());

	if(settings->contains("Pot_graph_enable"))
		ui.potChk->setChecked(settings->value("Pot_graph_enable", false).toBool());

	if(settings->contains("Flags_graph_enable"))
		ui.flagsChk->setChecked(settings->value("Flags_graph_enable", false).toBool());

	//положение окна
	if(settings->contains("position"))
		move(settings->value("position", QPoint(100, 200)).toPoint());

	if(settings->contains("size")){
		resize(settings->value("size", QSize(615, 260)).toSize());
	}

	//дефолтные пути
	default_load_path = settings->value("load_path", "").toString();
	default_save_path = settings->value("save_path", "").toString();

	settings->endGroup();

	//не даем включить графики, даже если они были включены в настройках
	setHardware(local_entype);
}

bool ChartDlg::setBreakLine(double t1, double t2)
{
	//обход существующих маркеров
	int i=0;
	for(i=0; i<10000-PLOTSNUM; i++){
		if(markers[i] == NULL) break;
		else if(markers[i]->xValue() == t1) return false;
	}

	for(int j=0; j<PLOTSNUM; j++){
		markers[i+j] = new QwtPlotMarker();
		InitMarker(markers[i+j], t1, (t2-t1)/delay);
		markers[i+j]->attach(plots[j]);

		static_cast<myScaleEngine *>(plots[j]->axisScaleEngine(QwtPlot::xTop))->add_ticks(t1);
		static_cast<myScaleEngine *>(plots[j]->axisScaleEngine(QwtPlot::xBottom))->add_ticks(t1);
	}
	markers[i+PLOTSNUM] = new QwtPlotMarker();
	InitMarker(markers[i+PLOTSNUM], t1, (t2-t1)/delay);
	markers[i+PLOTSNUM]->attach(plotmulti);
	static_cast<myScaleEngine *>(plotmulti->axisScaleEngine(QwtPlot::xTop))->add_ticks(t1);
	static_cast<myScaleEngine *>(plotmulti->axisScaleEngine(QwtPlot::xBottom))->add_ticks(t1);

	return true;
}

void ChartDlg::setFileChanged()
{
	if(fileopenLbl.isVisible() && !fileopenLbl.text().startsWith("*"))
		fileopenLbl.setText("*"+fileopenLbl.text());
}

void ChartDlg::setHardware(unsigned int entype)
{
	local_entype = entype;
	{
#ifndef SERVICEMODE
		if (entype == ENGINE_TYPE_STEP) {
			ui.voltCChk->setChecked(false);
			ui.voltCChk->setEnabled(false);

			ui.currCChk->setChecked(false);
			ui.currCChk->setEnabled(false);

			ui.pwmChk->setChecked(false);
			ui.pwmChk->setEnabled(false);
		} else if (entype == ENGINE_TYPE_DC) {
			ui.voltCChk->setChecked(false);
			ui.voltCChk->setEnabled(false);

			ui.currCChk->setChecked(false);
			ui.currCChk->setEnabled(false);

			ui.pwmChk->setEnabled(true);
		} else if (entype == ENGINE_TYPE_BRUSHLESS){
			ui.voltCChk->setEnabled(true);

			ui.currCChk->setEnabled(true);

			ui.pwmChk->setEnabled(true);
		}
#endif
	}
	update();
}

void ChartDlg::disableDeprecatedBoxes(bool value)
{
	ui.usbCurrentChk->setDisabled(value);
	ui.usbVoltageChk->setDisabled(value);
}
