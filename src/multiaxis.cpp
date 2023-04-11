#include "multiaxis.h"
#include "qglobal.h"


#define FIND(TYPE, NAME, NUM) this->findChild<TYPE *>(QString(NAME) + "_" + toStr(NUM+1))
#define INIT_MARKER(NAME, LINE_ORIENT, QPEN, ATT_PLOT) { \
QwtPlotMarker* qm = new QwtPlotMarker(); \
qm->setLineStyle(LINE_ORIENT); \
qm->setZ(1000); \
qm->setLinePen(QPEN); \
qm->attach(ATT_PLOT); \
qm->setVisible(true); \
NAME.push_back(qm); \
}

extern MessageLog* mlog;
extern bool save_configs;
using std::min;

class HiddenWidget : public QwtPlot {
public:
	HiddenWidget();
	void addtolist(QwtPlot* wgt);
	void clearlist();
private:
	QList<QwtPlot*> clients;
protected:
	void focusOutEvent(QFocusEvent* event);
	void focusInEvent(QFocusEvent* event);
};
HiddenWidget::HiddenWidget() : QwtPlot() {
}
void HiddenWidget::addtolist(QwtPlot* plot) {
	clients.append(plot);
}
void HiddenWidget::clearlist() {
	clients.clear();
}
void HiddenWidget::focusOutEvent(QFocusEvent* event) {
	Q_UNUSED(event)
	foreach (QwtPlot* item, clients) {
		item->setCanvasBackground(QColor(255,255,255));
	}
}
void HiddenWidget::focusInEvent(QFocusEvent* event) {
	Q_UNUSED(event)
	foreach (QwtPlot* item, clients) {
		item->setCanvasBackground(QColor(224,255,224)); // light green
	}
}
	HiddenWidget *hidden;



Multiaxis::Multiaxis(QWidget *parent, Qt::WFlags flags, QList<QString> _device_names, QList<DeviceInterface*> _devinterfaces)
	: QWidget(parent, flags)
{
	devinterfaces = _devinterfaces;
	devcount = min(devinterfaces.size(), max_devices);
	ui.setupUi(this);

	#ifdef __APPLE__ // #3084
	logDlg = new LogDlg(this, /*NULL,*/ "");
	#else
	logDlg = new LogDlg(this, "");
	logDlg->setWindowFlags(logDlg->windowFlags() | Qt::WindowMaximizeButtonHint | Qt::Tool);
	#endif
	logEdit = logDlg->findChild<QTableWidget*>("logEdit");
	InitTable();

	qRegisterMetaType<result_t>("result_t");
	qRegisterMetaType<measurement_t>("measurement_t");

	QBrush redBrush(QColor(255, 0, 0, 255));
	QBrush greyBrush(QColor(128, 128, 128, 255));
	QBrush greenBrush(QColor(0, 208, 0, 255));
	QBrush yellowBrush(QColor(255, 212, 0, 255));
	QBrush semitransparentBrush(QColor(0, 0, 0, 12));
	QBrush clearBrush(QColor(0, 0, 0, 0));

	QObject::connect(ui.pushButton_settings_1,	SIGNAL(clicked()), this, SLOT(OnSettings1BtnClicked()));
	QObject::connect(ui.pushButton_settings_2,	SIGNAL(clicked()), this, SLOT(OnSettings2BtnClicked()));
	QObject::connect(ui.pushButton_settings_3,	SIGNAL(clicked()), this, SLOT(OnSettings3BtnClicked()));

	QObject::connect(ui.pushButton_STOP,		SIGNAL(clicked()), this, SLOT(OnStopBtnPressed()));
	QObject::connect(ui.pushButton_softStop,	SIGNAL(clicked()), this, SLOT(OnSstpBtnPressed()));
	QObject::connect(ui.pushButton_moveTo,		SIGNAL(clicked()), this, SLOT(OnMoveBtnPressed()));
	QObject::connect(ui.pushButton_shiftOn,		SIGNAL(clicked()), this, SLOT(OnShiftBtnPressed()));
	QObject::connect(ui.pushButton_home,		SIGNAL(clicked()), this, SLOT(OnHomeBtnPressed()));
	QObject::connect(ui.pushButton_zero,		SIGNAL(clicked()), this, SLOT(OnZeroBtnPressed()));
	QObject::connect(ui.pushButton_scripting,	SIGNAL(clicked()), this, SLOT(OnScriptingBtnPressed()));
	QObject::connect(ui.pushButton_log,			SIGNAL(clicked()), this, SLOT(OnLogBtnPressed()));
	QObject::connect(ui.pushButton_calibration,	SIGNAL(clicked()), this, SLOT(OnCalibrationBtnPressed()));
	QObject::connect(ui.pushButton_exit,		SIGNAL(clicked()), this, SLOT(OnExitBtnPressed()));

	installEventFilter( this );

	for (int i=0; i<devcount; i++) {
		strncpy(device_name[i], _device_names.at(i).toLocal8Bit().data(), 256);
		updateThread = new UpdateThread(i, this, device_name[i], _devinterfaces.at(i));
		connect(updateThread, SIGNAL(done()), this, SLOT(UpdateState()));

		updateThread->start(QThread::HighestPriority);
		#ifdef __APPLE__ // #3084
		SettingsDlg* settingsDlg = new SettingsDlg(this, updateThread, devinterfaces.at(i), true);
		#else
		SettingsDlg* settingsDlg = new SettingsDlg(NULL, updateThread, devinterfaces.at(i), true);
		settingsDlg->setWindowFlags(settingsDlg->windowFlags() | Qt::Tool);
		#endif

		settingsDlgs.insert(i, settingsDlg);
		positionerNeedsUpdate[i] = true;
		noDevice[i] = true;
	}
	#ifdef __APPLE__ // #3084
	scriptDlg = new ScriptDlg(this, NULL, "");
	#else
	scriptDlg = new ScriptDlg(NULL, NULL, "");
	scriptDlg->setWindowFlags(scriptDlg->windowFlags() | Qt::WindowMaximizeButtonHint | Qt::Tool);
	#endif

	scriptDlg->InitUI();
	scriptStartBtn = scriptDlg->findChild<QPushButton*>("startBtn");
	scriptStopBtn = scriptDlg->findChild<QPushButton*>("stopBtn");
	scriptLoadBtn = scriptDlg->findChild<QPushButton*>("loadBtn");
	scriptSaveBtn = scriptDlg->findChild<QPushButton*>("saveBtn");
	codeEdit = scriptDlg->findChild<QTextEdit*>("codeEdit");
	connect(scriptDlg, SIGNAL(script_exception(QString)), this, SLOT(ScriptExceptionLog(QString)));

	scriptThreadMulti = new ScriptThreadMulti(this, devinterfaces);
	scriptThreadMulti->start(QThread::IdlePriority);
	connect(scriptDlg, SIGNAL(finished(int)), this, SLOT(OnScriptStopBtnPressed()));
	connect(scriptDlg, SIGNAL(start()),       this, SLOT(OnScriptStartBtnPressed()));
	connect(scriptDlg, SIGNAL(stop(bool)),    this, SLOT(OnScriptStopBtnPressed(bool)));
	connect(ui.pushButton_STOP, SIGNAL(clicked()), scriptDlg, SLOT(OnStopBtnClicked()));

	for (int i=0; i<devcount; i++) {
		QObject::connect(this, SIGNAL(SgnDisableStagePages(bool, int)), ((PagePositionerNameWgt*)settingsDlgs.at(i)->pageWgtsLst[PagePositionerNameNum]), SLOT(SetDisabled(bool)));
		QObject::connect(this, SIGNAL(SgnDisableStagePages(bool, int)), ((PageStageWgt*)settingsDlgs.at(i)->pageWgtsLst[PageStageNum]), SLOT(SetDisabled(bool)));
		QObject::connect(this, SIGNAL(SgnDisableStagePages(bool, int)), ((PageStageDCMotorWgt*)settingsDlgs.at(i)->pageWgtsLst[PageStageDCMotorNum]), SLOT(SetDisabled(bool)));
		QObject::connect(this, SIGNAL(SgnDisableStagePages(bool, int)), ((PageStageEncoderWgt*)settingsDlgs.at(i)->pageWgtsLst[PageStageEncoderNum]), SLOT(SetDisabled(bool)));
		QObject::connect(this, SIGNAL(SgnDisableStagePages(bool, int)), ((PageStageHallsensorWgt*)settingsDlgs.at(i)->pageWgtsLst[PageStageHallsensorNum]), SLOT(SetDisabled(bool)));
		QObject::connect(this, SIGNAL(SgnDisableStagePages(bool, int)), ((PageStageGearWgt*)settingsDlgs.at(i)->pageWgtsLst[PageStageGearNum]), SLOT(SetDisabled(bool)));
		QObject::connect(this, SIGNAL(SgnDisableStagePages(bool, int)), ((PageStageAccessoriesWgt*)settingsDlgs.at(i)->pageWgtsLst[PageStageAccessoriesNum]), SLOT(SetDisabled(bool)));
		emit SgnDisableStagePages(true, i);
	}

	saved_bk_color = this->palette().color(QPalette::Window);
	double k = 0.1;
	alarm_bk_color = QColor(min((double)255, saved_bk_color.red()*(1-k)+255*k), saved_bk_color.green()*(1-k), saved_bk_color.blue()*(1-k), 255);

	for (int i=0; i<max_devices; i++) {
		tr_matrix[i] = i; // translation array initialized to unity
		curPoss.push_back(FIND(QLabel, "curPos", i));
		tableLabel.push_back(FIND(QLabel, "tableLabel", i));
		moveTos.push_back(FIND(StepSpinBox, "spinBox_moveTo", i));
		shiftOns.push_back(FIND(StepSpinBox, "spinBox_shiftOn", i));
		joyDecs.push_back(FIND(QPushButton, "pushButton_joyDec", i));
		joyIncs.push_back(FIND(QPushButton, "pushButton_joyInc", i));
		joyDecs.last()->installEventFilter(this);
		joyIncs.last()->installEventFilter(this);
		groupBoxs.push_back(FIND(QGroupBox, "groupBox", i));
		voltageValues.push_back(FIND(QLabel, "voltageValue", i));
		currentValues.push_back(FIND(QLabel, "currentValue", i));
		speedValues.push_back(FIND(QLabel, "speedValue", i));
		commandValues.push_back(FIND(QLabel, "commandValue", i));
		powerValues.push_back(FIND(QLabel, "powerValue", i));
	}
	for (int i=0; i<devcount; i++) {
		moveTos.at(i)->setAlignment(Qt::AlignHCenter);
		shiftOns.at(i)->setAlignment(Qt::AlignHCenter);
		moveTos.at(i)->setMinimum(INT_MIN);
		shiftOns.at(i)->setMinimum(INT_MIN);
		moveTos.at(i)->setMaximum(INT_MAX);
		shiftOns.at(i)->setMaximum(INT_MAX);

		moveTos.at(i)->setUnitType(UserUnitSettings::TYPE_COORD);
		shiftOns.at(i)->setUnitType(UserUnitSettings::TYPE_COORD);
		moveTos.at(i)->setFloatStyleAllowed(true);
		shiftOns.at(i)->setFloatStyleAllowed(true);
		moveTos.at(i)->setTempChangeAllowed(false);
		shiftOns.at(i)->setTempChangeAllowed(false);
		moveTos.at(i)->setSerial(settingsDlgs.at(i)->controllerStgs->serialnumber);
		shiftOns.at(i)->setSerial(settingsDlgs.at(i)->controllerStgs->serialnumber);
		settingsDlgs.at(i)->setStyles(true); // can do this after movetos and shiftons received identifying serials (see directly above)
	}

	for (int i = 0; i < devcount; i++) {
		// #23168 change all spin boxes maximum values
		Q_FOREACH(QWidget* item, settingsDlgs.at(i)->pageWgtsLst) {
			Q_FOREACH(QSpinBox* sbox, item->findChildren<QSpinBox*>()) {
				if (dynamic_cast<StepSpinBox*>(sbox) == nullptr) { // we do not touch stepspinboxes, because now they can contain values in userunits: #17722
					sbox->setRange(0, INT_MAX);
				}
			}
		}
	}

	font = ui.label_curPos->font(); // uhm...

	LoadLineEraseDelay();

	unitsDlg = new DimensionalUnitsDlg(this, "Common settings", getSerials(), line_erase_delay);
	unitsDlg->setWindowFlags(unitsDlg->windowFlags() | Qt::WindowMaximizeButtonHint | Qt::Tool);
	connect(unitsDlg, SIGNAL(sendSerials(QStringList, double)), this, SLOT(receiveSerials(QStringList, double)));

	logStgs = new LogSettings();
	pagelog = new PageLogSettingsWgt(NULL, logStgs);
	unitsDlg->insertWidget(pagelog);

	LoadConfigs(); // needs valid scriptDlg, logDlg, pagelog

	hidden = new HiddenWidget(); // proxy for focus in/out events for plotXY and plotZ
	hidden->setFixedSize(0,0); // can't hide because hidden widgets don't receive focus events
	hidden->setFocusPolicy(Qt::StrongFocus);// Qt::NoFocus//Qt::StrongFocus

	plotXY = new QwtPlot(parent);
	plotXY->setCanvasBackground(Qt::white);
	((QwtPlotCanvas*)plotXY->canvas())->setFocusIndicator(QwtPlotCanvas::CanvasFocusIndicator);
	plotXY->canvas()->setFocusProxy( hidden );
	plotXY->canvas()->setFocusPolicy(Qt::NoFocus);
	hidden->addtolist(plotXY);

	plotZ = new QwtPlot(parent);
	plotZ->setAxisScale(QwtPlot::xBottom, -1, 1);
	plotZ->enableAxis(QwtPlot::xBottom, false);
	if (devcount > 2) {
		plotZ->setCanvasBackground(Qt::white);
		((QwtPlotCanvas*)plotZ->canvas())->setFocusIndicator(QwtPlotCanvas::CanvasFocusIndicator);
		plotZ->canvas()->setFocusProxy( hidden );
		plotZ->canvas()->setFocusPolicy(Qt::NoFocus);
		hidden->addtolist(plotZ);
	} else {
		plotZ->setCanvasBackground(this->palette().color(QWidget::backgroundRole()));
	}
	//plotZ->canvas()->setFocusPolicy(Qt::NoFocus);

	this->layout()->addWidget(hidden);

	plots << plotXY << plotXY << plotZ;
	axes << QwtPlot::xBottom << QwtPlot::yLeft << QwtPlot::yLeft;

	dotXY = new QwtPlotCurve();
	dotXY->setPen(QPen(QBrush(QColor(0,0,0)), 5));
	dotXY->setStyle(QwtPlotCurve::Dots);
	dotXY->attach(plotXY);
	dotZ = new QwtPlotCurve();
	dotZ->setPen(QPen(QBrush(QColor(0,0,0)), 5));
	dotZ->setStyle(QwtPlotCurve::Lines);
	dotZ->attach(plotZ);

	trace = new QwtPlotCurve();
	trace->setPen(QPen(QBrush(QColor(0,0,255)), 2));
	trace->setStyle(QwtPlotCurve::Lines);
	trace->attach(plotXY);
	trace->setVisible(true);

	tgtXY = new QwtPlotCurve();
	tgtXY->setPen(QPen(QBrush(QColor(0,0,0)), 7));
	tgtXY->setStyle(QwtPlotCurve::Dots);
	tgtXY->attach(plotXY);
	tgtZ = new QwtPlotCurve();
	tgtZ->setPen(QPen(QBrush(QColor(0,0,0)), 7));
	tgtZ->setStyle(QwtPlotCurve::Lines);
	tgtZ->attach(plotZ);

	QwtPlotGrid *gridXY = new QwtPlotGrid();
	gridXY->setPen(QPen(QColor(200,200,200)));
	gridXY->setMinPen(QPen(QColor(230,230,230)));
	gridXY->setMajPen(QPen(QColor(200,200,200)));
	gridXY->enableXMin(true);
	gridXY->enableYMin(true);
	gridXY->attach(plotXY);
	QwtPlotGrid *gridZ = new QwtPlotGrid();
	gridZ->setPen(QPen(QColor(200,200,200)));
	gridZ->setMinPen(QPen(QColor(230,230,230)));
	gridZ->setMajPen(QPen(QColor(200,200,200)));
	gridZ->enableX(false);
	gridZ->enableXMin(false);
	gridZ->enableYMin(true);
	gridZ->attach(plotZ);

	edge_width = 4;
	QPen pen = QPen(QBrush(QColor(192,0,0)), edge_width);
	INIT_MARKER(markmin, QwtPlotMarker::VLine, pen, plotXY);
	INIT_MARKER(markmin, QwtPlotMarker::HLine, pen, plotXY);
	INIT_MARKER(markmin, QwtPlotMarker::HLine, pen, plotZ);
	INIT_MARKER(markmax, QwtPlotMarker::VLine, pen, plotXY);
	INIT_MARKER(markmax, QwtPlotMarker::HLine, pen, plotXY);
	INIT_MARKER(markmax, QwtPlotMarker::HLine, pen, plotZ);

	int color = 64;
	QPen grey = QPen(QColor(color,color,color));
	INIT_MARKER(mark, QwtPlotMarker::VLine, grey, plotXY);
	INIT_MARKER(mark, QwtPlotMarker::HLine, grey, plotXY);
	if (devcount > 2) {
		INIT_MARKER(mark, QwtPlotMarker::HLine, grey, plotZ);
	}
	for (int i=devinterfaces.size(); i<max_devices; i++) { // In fact, i == 2 is the only possible case here, but let's preserve the generality
		curPoss.at(i)->hide();
		//tableLabel.at(i)->setPalette(palette_green);
		tableLabel.at(i)->setDisabled(true);
		moveTos.at(i)->setDisabled(true);
		shiftOns.at(i)->setDisabled(true);
		joyDecs.at(i)->setDisabled(true);
		joyIncs.at(i)->setDisabled(true);
		groupBoxs.at(i)->setDisabled(true);
		markmin.at(i)->setVisible(false);
		markmax.at(i)->setVisible(false);
	}

	QwtPlotPicker* pickerXY = new /*My*/QwtPlotPicker((QwtPlotCanvas*)plotXY->canvas());
	pickerXY->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ControlModifier);
	connect(pickerXY, SIGNAL(selected(const QPointF &)), this, SLOT(OnPickerXYPointSelected(const QPointF &)));
	pickerXY->setStateMachine(new QwtPickerClickPointMachine());

	if (devcount > 2) {
		QwtPlotPicker* pickerZ = new /*My*/QwtPlotPicker((QwtPlotCanvas*)plotZ->canvas());
		pickerZ->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton, Qt::ControlModifier);
		connect(pickerZ, SIGNAL(selected(const QPointF &)), this, SLOT(OnPickerZPointSelected(const QPointF &)));
		pickerZ->setStateMachine(new QwtPickerClickPointMachine());
	}

	palette_green.setBrush(QPalette::Active, QPalette::WindowText, greenBrush);
	palette_green.setBrush(QPalette::Inactive, QPalette::WindowText, greenBrush);
	palette_green.setBrush(QPalette::Disabled, QPalette::WindowText, semitransparentBrush);
	palette_green.setBrush(QPalette::Light, clearBrush); // For Windows XP & 2003

	((QGridLayout*)ui.groupBox_Joystick->layout())->addWidget(plotXY, 1, 1);
	((QGridLayout*)ui.groupBox_Joystick->layout())->addWidget(plotZ, 1, 4);
	plotZ->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	plotZ->setMinimumWidth(100);

	QTimer* timer = new QTimer();
	timer->setInterval(33);
	connect( timer, SIGNAL(timeout()), SLOT(UpdateState()) );
	timer->start();

	QApplication::setDoubleClickInterval(0);

	exitBox = new InfoBox(this);
	exitBox->setIcon(QMessageBox::Information);
	exitBox->setButtons(QDialogButtonBox::Cancel);
	exitBox->setText("Stopping, please wait...");
	exitBox->setClosingOnReject(false);

	hidden->setFocusPolicy(Qt::NoFocus);// Qt::NoFocus//Qt::StrongFocus
	
	exiting = false;

	for (int i = 0; i < max_devices; i++) {
		tableLabel.at(i)->setPalette(palette_green);
		tableLabel.at(i)->setDisabled(true);
	}

	LoadSingleConfigs1(); // needs getSerials(), which needs translate(), which is initialized above

	// Feature #64766 In multi-axis XiLab, when opened, coordinates are displayed only in steps
	for (int i = 0; i<devcount; i++) {
		if (settingsDlgs.at(i)->AllPagesFromDeviceToClassToUi() == true)
			settingsDlgs.at(i)->AllPagesFromUiToClassToDevice();
	}

	inited = true;
}

Multiaxis::~Multiaxis()
{
}

void Multiaxis::closeEvent(QCloseEvent *event)
{
	if (save_configs) {
		SaveAxisConfig();
		SaveSingleConfigs();
		SaveConfigs();
	}
	for(int i=0; i<devcount; i++) {
		settingsDlgs.at(i)->updateThread->wait_for_exit = true;
		disconnect(settingsDlgs.at(i)->updateThread);
		while (settingsDlgs.at(i)->updateThread->isRunning()) {
			msleep(20);
		}
	}
	for (int i=0; i<devinterfaces.size(); i++) {
		if (devinterfaces.at(i)->is_open()) {
			devinterfaces.at(i)->close_device();
		}
	}
	event->accept();
}

void Multiaxis::Log(QString string, QString source, int loglevel)
{
	QDateTime now = QDateTime::currentDateTime();
	emit InsertLineSgn(now, string, source, loglevel, logStgs);
}

void Multiaxis::ScriptExceptionLog(QString message)
{
	Log(message, SOURCE_SCRIPT, LOGLEVEL_ERROR);
}

inline void clearEditableFlag(QTableWidgetItem *item)
{
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
}

void Multiaxis::UpdateLogTable()
{
	double tick1, tick2;
	tick2 = tick1 = t.getElapsedTimeInMilliSec();
	bool do_autoscroll = (logEdit->verticalScrollBar()->sliderPosition() == logEdit->verticalScrollBar()->maximum());

	// Update log
	LogItem next_item;
	QString message_old = "";
	int counter_mess = 1;
	int row1 = logEdit->rowCount();

	if (row1 > 0) {
		row1--;
		message_old = logEdit->item(row1, 3)->text();
	}
	int inddupl = message_old.lastIndexOf(" [duplicated ");
	int indint = message_old.lastIndexOf(" times]");
	if ((inddupl > -1)) {
		QString strint = message_old.mid(inddupl + 13, indint - (inddupl + 13));
		counter_mess = strint.toInt();
		message_old.truncate(inddupl);
	}

	while (mlog->pop(&next_item) && (tick2 - tick1 < 50) ) { // gathers data for 50ms max (out of 100ms limit for a single update)
		tick2 = t.getElapsedTimeInMilliSec();
		QDateTime datetime = next_item.datetime;
		QString message = next_item.message;
		QString source = next_item.source;
		int loglevel = next_item.loglevel;

		QString date = datetime.toString("yyyy.MM.dd");
		QString time = datetime.toString("hh:mm:ss.zzz");
		LogSettings* ls = logStgs;

		if ((source == SOURCE_XILAB && loglevel <= ls->xll_index) || 
			(source == SOURCE_SCRIPT && loglevel <= ls->sll_index) ||
			(source == SOURCE_LIBRARY && loglevel <= ls->lll_index)) {

			int row = logEdit->rowCount();

			if (message_old != message) {
				logEdit->insertRow(row);
				logEdit->setRowCount(row + 1);
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
			logEdit->setItem(row, 0, item0);
			QTableWidgetItem *item1 = new QTableWidgetItem(time);
			clearEditableFlag(item1);
			logEdit->setItem(row, 1, item1);
			QTableWidgetItem *item2 = new QTableWidgetItem(source);
			clearEditableFlag(item2);
			logEdit->setItem(row, 2, item2);
			QString count = "";
			if (counter_mess > 1)
				count = " [duplicated " + QString::number(counter_mess) + " times]";
			QTableWidgetItem *item3 = new QTableWidgetItem((QString)(message + count));
			clearEditableFlag(item3);
			item3->setTextColor(color);
			logEdit->setItem(row, 3, item3);

			logEdit->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
			logEdit->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
			logEdit->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);
			logEdit->horizontalHeader()->setResizeMode(3, QHeaderView::Stretch);

		}
	}
	if (do_autoscroll)
		logEdit->scrollToBottom();
}

void Multiaxis::InitTable()
{
	logEdit->setColumnCount(4);
	headerLabels.clear();
	headerLabels << QString("Date") << QString("Time") << QString("Source") << QString("Message");
	logEdit->setHorizontalHeaderLabels(headerLabels);
	logEdit->verticalHeader()->setVisible(false);

	logEdit->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

	QFontMetrics fm (logEdit->font());
	int h = fm.height() + 2;
	logEdit->verticalHeader()->setDefaultSectionSize( h );

	logEdit->setStyleSheet("QTableWidget { gridline-color: #F0F0F0 }");
}

void Multiaxis::UpdateState() {
	UpdateLogTable();
	if (exiting) {
		int mvcmd = 0;
		for (int i=0; i<devcount; i++)
			mvcmd |= devinterfaces.at(i)->cs->status().MvCmdSts; ///state_vars[i].state.MvCmdSts; // accumulate all bits
		if (mvcmd == MVCMD_SSTP) { // All stopped OK, let's exit
			if (exitBox != NULL) {
				if (exitBox->isVisible()) {
					for (int i=0; i<devcount; i++) {
						if (devinterfaces.at(i)->command_power_off() != result_ok) {
							Log("Error sending power off command on exit.", SOURCE_XILAB, LOGLEVEL_WARNING);
							return;
						}
					}
					delete exitBox;
					exitBox = NULL;
					close();
					return;
				} else {
					exiting = false;
				}
			}
		}
		else if (mvcmd == MVCMD_SSTP + MVCMD_RUNNING) { // At least one is still stopping, no commands other that SSTP, let's do nothing
			;
		}
		else { // someone was naughty; let's error
			exiting = false;
			exitBox->close();
			QMessageBox::warning(this, tr("XILab"), tr("Exit aborted because of external command interrupt (joystick, sync in, etc.)."), QMessageBox::Ok);
		}
	}
	int wd = plotXY->axisWidget(QwtPlot::yLeft)->width();
	int ht = plotXY->axisWidget(QwtPlot::xBottom)->height();
	ui.axis_label_Y->setFixedWidth(wd);
	ui.axis_label_X->setFixedHeight(ht);
	ui.axis_label_Y_invis->setFixedWidth(wd);
	ui.axis_label_X_invis->setFixedHeight(ht);
	ui.axis_label_Z->setVisible(false);

	QString wndTitle = "XILab multiaxis ", title;
	for (int e=0; e<devcount; e++) {
		int i = translate(e);

		QString units = settingsDlgs.at(i)->uuStgs->unitName;

		QPen pen = mark.at(i)->linePen();
		if (devinterfaces.at(i)->cs->connect() == false && noDevice[i] == false) {
			groupBoxs.at(e)->setDisabled(true);
			curPoss.at(e)->setDisabled(true);
			tableLabel.at(e)->setDisabled(true);
			joyDecs.at(e)->setDisabled(true);
			joyIncs.at(e)->setDisabled(true);
			moveTos.at(i)->setDisabled(true); // (i) is intentional here
			shiftOns.at(i)->setDisabled(true); // because we reshuffle the boxes
			pen.setStyle(Qt::DashLine);
			mark.at(e)->setLinePen(pen);
			noDevice[i] = true;
			continue;
		} else if (devinterfaces.at(i)->cs->connect()== true && noDevice[i] == true){
			groupBoxs.at(e)->setDisabled(false);
			curPoss.at(e)->setDisabled(false);
			tableLabel.at(e)->setDisabled(false);
			joyDecs.at(e)->setDisabled(false);
			joyIncs.at(e)->setDisabled(false);
			moveTos.at(i)->setDisabled(false); // (i) is intentional here
			shiftOns.at(i)->setDisabled(false); // because we reshuffle the boxes
			settingsDlgs.at(i)->motorStgs->LoadFirstInfo(); // put this before setStepFactors
			moveTos.at(i)->setStepFactor(settingsDlgs.at(i)->motorStgs->getStepFrac());
			shiftOns.at(i)->setStepFactor(settingsDlgs.at(i)->motorStgs->getStepFrac());
			pen.setStyle(Qt::SolidLine);
			mark.at(e)->setLinePen(pen);
			noDevice[i] = false;
		} else if (devinterfaces.at(i)->cs->connect() == false && noDevice[i] == true)
			continue;

		if ((devinterfaces.at(i)->cs->status().Flags & STATE_EEPROM_CONNECTED) && devinterfaces.at(i)->cs->connect() && !noDevice[i]) {
			if (positionerNeedsUpdate[i] == true) {
				result_t result = devinterfaces.at(i)->get_stage_name(&settingsDlgs.at(i)->stageStgs->stage_name);
				positionerNeedsUpdate[i] = (result != result_ok);
				if (settingsDlgs.at(i)->isVisible() == true) {
					settingsDlgs.at(i)->StageSettingsFromDeviceToClassToUi();
					emit SgnDisableStagePages(false, i);
				} else {
					settingsDlgs.at(i)->stageStgs->need_load = true;
				}
			}
		} else {
			if (positionerNeedsUpdate[i] == false) {
				settingsDlgs.at(i)->stageStgs->stage_name.PositionerName[0] = '\0'; // erases username on disconnect
				emit SgnDisableStagePages(true, i);
				if (settingsDlgs.at(i)->isVisible() == false)
					settingsDlgs.at(i)->stageStgs->need_load = false;
			}
			positionerNeedsUpdate[i] = true;
		}

		if ((strlen(settingsDlgs.at(i)->stageStgs->stage_name.PositionerName) > 0) && (devinterfaces.at(i)->cs->status().Flags & STATE_EEPROM_CONNECTED))
			title = ("[" + QString::fromUtf8(settingsDlgs.at(i)->stageStgs->stage_name.PositionerName) + "]");
		else if (strlen(settingsDlgs.at(i)->motorStgs->name.ControllerName) > 0)
			title = (QString::fromUtf8(settingsDlgs.at(i)->motorStgs->name.ControllerName));
		else 
			title = (QString(settingsDlgs.at(i)->controllerStgs->device_info.ManufacturerId)
							   + toStr(settingsDlgs.at(i)->controllerStgs->serialnumber));
		wndTitle += title + ((e<devcount-1) ? ", " : "");
		QString axis;
		if (e == 0) axis = "X";
		else if (e == 1) axis = "Y";
		else if (e == 2) axis = "Z";
		groupBoxs.at(e)->setTitle("Motor " + axis + ", " + QString(settingsDlgs.at(i)->controllerStgs->device_info.ManufacturerId)
														 + toStr(settingsDlgs.at(i)->controllerStgs->serialnumber));

		bool currentStateIsAlarm = ((devinterfaces.at(i)->cs->status().Flags & STATE_ALARM) != 0);
		if (previousStateIsAlarm[settingsDlgs.at(i)->controllerStgs->serialnumber] != currentStateIsAlarm)
			groupBoxs.at(i)->setStyleSheet("QGroupBox { background-color: " + QString(currentStateIsAlarm ? alarm_bk_color.name() : saved_bk_color.name()) + " ; }");
		previousStateIsAlarm[settingsDlgs.at(i)->controllerStgs->serialnumber] = currentStateIsAlarm;

		if(devinterfaces.at(i)->getMode() == BOOTLOADER_MODE)
		{
			curPoss.at(e)->setText(tr("Bootloader v%1.%2.%3").arg(settingsDlgs.at(i)->controllerStgs->bootloader_version.major).arg(settingsDlgs.at(i)->controllerStgs->bootloader_version.minor).arg(settingsDlgs.at(i)->controllerStgs->bootloader_version.release));
			voltageValues.at(e)->setText("-");
			currentValues.at(e)->setText("-");
			speedValues.at(e)->setText("-");
			commandValues.at(e)->setText("-");
			powerValues.at(e)->setText("-");
			continue;
		}

		QString value;
		curPoss.at(e)->setText(settingsDlgs.at(i)->getFormattedPosition());

		if (settingsDlgs.at(i)->uuStgs->correctionTable == "\"\"")
		{
			tableLabel.at(e)->setPalette(palette_green);
			tableLabel.at(e)->setDisabled(true);
			tableLabel.at(e)->setToolTip(tr("Loading correction table status"));
		}
		else
		{
			if (settingsDlgs.at(i)->uuStgs->enable)
			{
				tableLabel.at(e)->setPalette(palette_green);
				tableLabel.at(e)->setEnabled(true);
			}
			else
			{
				tableLabel.at(e)->setPalette(palette_green);
				tableLabel.at(e)->setDisabled(true);
			}

			tableLabel.at(e)->setToolTip(tr("Loading correction table status \n") + settingsDlgs.at(i)->uuStgs->correctionTable);
		}

		speedValues.at(e)->setText(settingsDlgs.at(i)->getFormattedSpeed());

		voltageValues.at(e)->setText(QString("%1 V").arg(devinterfaces.at(i)->cs->status().Upwr/100.));
		currentValues.at(e)->setText(QString("%1 mA").arg(devinterfaces.at(i)->cs->status().Ipwr));
		switch (devinterfaces.at(i)->cs->status().MvCmdSts & MVCMD_NAME_BITS)
		{
			case MVCMD_MOVE : value = tr("Move to"); break;
			case MVCMD_MOVR : value = tr("Shift on"); break;
			case MVCMD_LEFT : value = tr("Move left"); break;
			case MVCMD_RIGHT: value = tr("Move right"); break;
			case MVCMD_STOP : value = tr("Stop"); break;
			case MVCMD_HOME : value = tr("Homing"); break;
			case MVCMD_LOFT : value = tr("Loft"); break;
			case MVCMD_SSTP : value = tr("Soft stop"); break;
			default         : value = tr("Unknown");
		}
		commandValues.at(e)->setText(value);
		font.setBold(devinterfaces.at(i)->cs->status().MvCmdSts & MVCMD_RUNNING);
		commandValues.at(e)->setFont(font);

		switch (devinterfaces.at(i)->cs->status().PWRSts)
		{
			case PWR_STATE_OFF		: value = tr("Off"); break;
			case PWR_STATE_NORM		: value = tr("Norm"); break;
			case PWR_STATE_REDUCT	: value = tr("Reduc"); break;
			case PWR_STATE_MAX		: value = tr("Max"); break;
			case PWR_STATE_UNKNOWN	: value = tr("Uknwn"); break;
			default                 : value = tr("Unknown");
		}
		powerValues.at(e)->setText(value);
	}
	setWindowTitle(wndTitle);
	double k;

	double x, y, z[2], w[2];
	int i;

	QDateTime last_update_time = QDateTime::currentDateTimeUtc();
	for (int e=0; e<devcount; e++) {
		int i = translate(e);

		if (settingsDlgs.at(i)->uuStgs->enable)
			k = settingsDlgs.at(i)->uuStgs->getUnitPerStep();
		else
			k = 1;		

		double left, right;
		if (settingsDlgs.at(i)->sliderStgs->loadfromdevice) {
			left = k*(settingsDlgs.at(i)->motorStgs->edges.LeftBorder + settingsDlgs.at(i)->motorStgs->edges.uLeftBorder/settingsDlgs.at(i)->motorStgs->getStepFrac());
			right = k*(settingsDlgs.at(i)->motorStgs->edges.RightBorder + settingsDlgs.at(i)->motorStgs->edges.uRightBorder/settingsDlgs.at(i)->motorStgs->getStepFrac());
		} else {
			left = k*settingsDlgs.at(i)->sliderStgs->left;
			right = k*settingsDlgs.at(i)->sliderStgs->right;
		}
		if ((LeftBorder[i] != left) || (RightBorder[i] != right))
			plots.at(e)->setAxisScale(axes.at(e), left, right);
		LeftBorder[i] = left;
		RightBorder[i] = right;

		double curpos = k*(devinterfaces.at(i)->cs->status().CurPosition + (double)devinterfaces.at(i)->cs->status().uCurPosition / settingsDlgs.at(i)->motorStgs->getStepFrac());
		if (e == 0) {
			x = curpos;
		}
		if (e == 1) {
			y = curpos;
		}
		if (e == 2) {
			z[0] = z[1] = curpos;
			w[0] = -0.5; w[1] = 0.5;
		}
	}
	dotXY->setSamples(&x, &y, 1);
	if (devcount > 2)
		dotZ->setSamples(&w[0], &z[0], 2);

	if (x != saved_x || y != saved_y) {
		Point p;
		p.x = x;
		p.y = y;
		p.t = last_update_time;
		points.push_front(p);

		saved_x = x;
		saved_y = y;
	}
	
	const int cutoff = 1000; // cutoff point is 100 seconds at 1 update/100 ms or 1000 points.
	if (points.size() > (int)cutoff) { // truncate arrays if they become too large
		points.pop_back();
	}

	if (points.size() > 0) { // todo: if graphing is enabled
		unsigned int start = 0;
		x_array.clear();
		y_array.clear();
		t_array.clear();
		QDateTime currentTime = QDateTime::currentDateTime();
		while (  (qAbs(points.at(start).t.msecsTo(currentTime)) < line_erase_delay*1000) && (start+1 < points.size())  ) {
			Point *p = &points.at(start);
			x_array.push_back(p->x);
			y_array.push_back(p->y);
			t_array.push_back(p->t);
			start++;
		}
		if (t_array.size() > 0)
			trace->setRawSamples(&x_array[0], &y_array[0], static_cast<int>(t_array.size()) ); // thank vector for being continuous in memory
	}

	int e;
	e = 0;
	i = translate(e);
	mark.at(e)->setXValue(x);
	markmin.at(e)->setXValue(LeftBorder[i]);
	markmax.at(e)->setXValue(RightBorder[i]);

	e = 1;
	i = translate(e);	
	mark.at(e)->setYValue(y);
	markmin.at(e)->setYValue(LeftBorder[i]);
	markmax.at(e)->setYValue(RightBorder[i]);

	if (devcount > 2) {
		e = 2;
		i = translate(e);
		mark.at(e)->setYValue(z[0]);
		markmin.at(e)->setYValue(LeftBorder[i]);
		markmax.at(e)->setYValue(RightBorder[i]);
	}

	for (int e=0; e<devcount; e++) {
		int i = translate(e);
		markmin.at(e)->setVisible(devinterfaces.at(i)->cs->status().GPIOFlags & STATE_LEFT_EDGE);
		markmax.at(e)->setVisible(devinterfaces.at(i)->cs->status().GPIOFlags & STATE_RIGHT_EDGE);
	}

	plotXY->replot();
	if (devcount > 2)
		plotZ->replot();

	// multi-script
	if (scriptThreadMulti != NULL) {
		bool evaling = scriptThreadMulti->qengine->isEvaluating();
		scriptStartBtn->setDisabled(evaling);
		scriptStopBtn->setEnabled(evaling);
		if (!evaling)
			scriptStopBtn->setText("Stop");
		scriptLoadBtn->setDisabled(evaling);
		scriptSaveBtn->setDisabled(evaling);
		codeEdit->setTextInteractionFlags(evaling ? Qt::NoTextInteraction : Qt::TextEditorInteraction);
	}
	static int fixfocus = 0;
	if (QApplication::keyboardModifiers() == Qt::ControlModifier)
	{
		// ≈сли нажата клавиша CTRL этот код выполнитс€
		plotXY->canvas()->setFocusProxy(hidden);
		plotXY->canvas()->setFocus();
		plotZ->canvas()->setFocusProxy(hidden);
		plotZ->canvas()->setFocus();
		fixfocus = 1;
	}
	else
	{
		if (fixfocus) {
			this->focusPreviousChild();
			this->focusPreviousChild();
			this->focusPreviousChild();
			plotXY->canvas()->setFocusPolicy(Qt::NoFocus);
			plotZ->canvas()->setFocusPolicy(Qt::NoFocus);
			fixfocus = 0;
		}
	}

	for (int e = 0; e < devcount; e++) {
		int i = translate(e);
		// The block for displaying error messages of the correction table.
		switch (settingsDlgs.at(i)->uuStgs->messageType)
		{
		case 1:
			settingsDlgs.at(i)->uuStgs->messageType = 0;
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this, groupBoxs.at(i)->title(), settingsDlgs.at(i)->uuStgs->messageText,
				QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::Yes)
				((PageUserUnitsWgt*)settingsDlgs.at(i)->pageWgtsLst[PageUserUnitsNum])->OnCloseTableBtnClicked();
			break;
		case 3:
			settingsDlgs.at(i)->uuStgs->messageType = 0;
			QMessageBox::warning(this, groupBoxs.at(i)->title(), settingsDlgs.at(i)->uuStgs->messageText,
				QMessageBox::Ok);

			break;
		case 2:
		case 4:
		default:
			break;
		}
	}
}

void Multiaxis::OnMoveBtnPressed() {
	for (int i=0; i<devcount; i++) {
		if (settingsDlgs.at(i)->uuStgs->enable) {
			calibration_t calb;
			calb.A = settingsDlgs.at(i)->uuStgs->getUnitPerStep();
			calb.MicrostepMode = settingsDlgs.at(i)->motorStgs->engine.MicrostepMode;
			devinterfaces.at(i)->command_move_calb(moveTos.at(i)->value(), calb);
		} else {
			devinterfaces.at(i)->command_move(moveTos.at(i)->getStep(), moveTos.at(i)->getUStep());
		}
	}
	double x = moveTos.at(0)->value();
	double y = moveTos.at(1)->value();
	tgtXY->setSamples(&x, &y, 1);
	tgtXY->show();
}

void Multiaxis::OnShiftBtnPressed() {
	tgtXY->hide();
	tgtZ->hide();
	for (int i=0; i<devcount; i++) {
		if (settingsDlgs.at(i)->uuStgs->enable) {
			calibration_t calb;
			calb.A = settingsDlgs.at(i)->uuStgs->getUnitPerStep();
			calb.MicrostepMode = settingsDlgs.at(i)->motorStgs->engine.MicrostepMode;
			devinterfaces.at(i)->command_movr_calb(shiftOns.at(i)->value(), calb);
		} else {
			devinterfaces.at(i)->command_movr(shiftOns.at(i)->getStep(), shiftOns.at(i)->getUStep());
		}
	}
}

void Multiaxis::OnSstpBtnPressed() {
	tgtXY->hide();
	tgtZ->hide();
	for (int i=0; i<devcount; i++)
		if ((devinterfaces.at(i)->cs->status().MvCmdSts & MVCMD_RUNNING) == 0) { // Stop Lv.3 - PWOF
			devinterfaces.at(i)->command_power_off();
		} else if (devinterfaces.at(i)->cs->status().MvCmdSts == (MVCMD_SSTP + MVCMD_RUNNING)) { // Stop Lv.2 - STOP
			devinterfaces.at(i)->command_stop();
		} else { // Stop Lv.1 - SSTP
			devinterfaces.at(i)->command_sstp();
		}

}

void Multiaxis::OnStopBtnPressed() {
	tgtXY->hide();
	tgtZ->hide();
	for (int i=0; i<devcount; i++)
		devinterfaces.at(i)->command_stop();
}

void Multiaxis::OnZeroBtnPressed() {
	tgtXY->hide();
	tgtZ->hide();
	for (int i=0; i<devcount; i++)
		devinterfaces.at(i)->command_zero();
}

void Multiaxis::OnHomeBtnPressed() {
	tgtXY->hide();
	tgtZ->hide();
	for (int i=0; i<devcount; i++)
		devinterfaces.at(i)->command_home();
}

void Multiaxis::OnScriptStartBtnPressed() {
	codeEdit->setTextInteractionFlags(Qt::NoTextInteraction);
	scriptThreadMulti->do_eval = true;
}

void Multiaxis::OnScriptStopBtnPressed()
{
	OnScriptStopBtnPressed(false);
}

void Multiaxis::OnScriptStopBtnPressed(bool hardstop) {
	if (hardstop) {
		ScriptThreadMulti *newst = new ScriptThreadMulti(this, devinterfaces);
		ScriptThreadMulti *oldst = scriptThreadMulti;
		scriptThreadMulti = newst;
		p_scriptThread = newst;
		oldst->terminate(); // will memleak by design
		newst->start(QThread::IdlePriority);
		codeEdit->setTextInteractionFlags(Qt::TextEditorInteraction);
		scriptStopBtn->setText("Stop");
	} else {
		scriptThreadMulti->qengine->abortEvaluation();
		if (!scriptThreadMulti->qengine->isEvaluating())
			codeEdit->setTextInteractionFlags(Qt::TextEditorInteraction);
		else
			QTimer::singleShot(200, this, SLOT(CheckForceStopCondition()));
	}
}

void Multiaxis::CheckForceStopCondition() {
	if (scriptThreadMulti->qengine->isEvaluating())
		scriptStopBtn->setText("Force stop");
}

void Multiaxis::OnLogBtnPressed() {
	logDlg->show();
}

void Multiaxis::OnClearLogBtnPressed() {
	mlog->Clear();
	logEdit->clear();
	logEdit->setRowCount(0);
	InitTable();
}


void Multiaxis::OnScriptingBtnPressed() {
	scriptDlg->show();
	scriptDlg->raise();
	scriptDlg->activateWindow();
}

void Multiaxis::OnCalibrationBtnPressed() {
	unitsDlg->exec();
}

void Multiaxis::OnExitBtnPressed() {
	exiting = true;
	scriptThreadMulti->qengine->abortEvaluation();
	bool connect = true, mode_firmware = true;
	for (int i=0; i<devcount; i++) {
		connect &= devinterfaces.at(i)->cs->connect();
		mode_firmware &= (devinterfaces.at(i)->getMode() == FIRMWARE_MODE);
	}
	if (connect && mode_firmware) {
		for (int i=0; i<devcount; i++) {
			result_t result = devinterfaces.at(i)->command_sstp(); // first SSTP to initiate stopping
			if (result != result_ok) {
				Log("Error sending soft stop command on exit.", SOURCE_XILAB, LOGLEVEL_WARNING);
				return;
			}
		}
		msleep(UPDATE_INTERVAL); // we have to wait for one update of the updatethread
		if (exitBox != NULL)
			exitBox->exec();
	} else {
		close();
	}

}

void Multiaxis::OnSettings1BtnClicked() {
	int j = translate(0);
	if(settingsDlgs.at(j)->AllPagesFromDeviceToClassToUi() == true)
		settingsDlgs.at(j)->show();
	settingsDlgs.at(j)->raise();
	settingsDlgs.at(j)->activateWindow();
}

void Multiaxis::OnSettings2BtnClicked() {
	int j = translate(1);
	if(settingsDlgs.at(j)->AllPagesFromDeviceToClassToUi() == true)
		settingsDlgs.at(j)->show();
	settingsDlgs.at(j)->raise();
	settingsDlgs.at(j)->activateWindow();
}

void Multiaxis::OnSettings3BtnClicked() {
	int j = translate(2);
	if(settingsDlgs.at(j)->AllPagesFromDeviceToClassToUi() == true)
		settingsDlgs.at(j)->show();
	settingsDlgs.at(j)->raise();
	settingsDlgs.at(j)->activateWindow();
}

void Multiaxis::keyPressEvent( QKeyEvent* event ) {
	int i;
	if (event->isAutoRepeat())
		event->accept();

	if (QApplication::keyboardModifiers() == Qt::ControlModifier)	
	switch ( event->key() ) {
	case Qt::Key_Left:
		i = translate(0);
		devinterfaces.at(i)->command_left();
		tgtXY->hide();
		break;
	case Qt::Key_Right:
		i = translate(0);
		devinterfaces.at(i)->command_right();
		tgtXY->hide();
		break;
	case Qt::Key_Down:
		i = translate(1);
		devinterfaces.at(i)->command_left();
		tgtXY->hide();
		break;
	case Qt::Key_Up:
		i = translate(1);
		devinterfaces.at(i)->command_right();
		tgtXY->hide();
		break;
	case Qt::Key_PageDown:
		if (devcount > 2) {
			i = translate(2);
			devinterfaces.at(i)->command_left();
			tgtZ->hide();
		}
		break;
	case Qt::Key_PageUp:
		if (devcount > 2) {
			i = translate(2);
			devinterfaces.at(i)->command_right();
			tgtZ->hide();
		}
		break;
	default:
		event->ignore();
		break;
	}
}

void Multiaxis::keyReleaseEvent( QKeyEvent* event ) {
	int i;
	switch ( event->key() ) {
	case Qt::Key_Left:
	case Qt::Key_Right:
		i = translate(0);
		devinterfaces.at(i)->command_sstp();
		tgtXY->hide();
		break;
	case Qt::Key_Up:
	case Qt::Key_Down:
		i = translate(1);
		devinterfaces.at(i)->command_sstp();
		tgtXY->hide();
		break;
	case Qt::Key_PageUp:
	case Qt::Key_PageDown:
		if (devcount > 2) {
			i = translate(2);
			devinterfaces.at(i)->command_sstp();
			tgtZ->hide();
		}
		break;
	default:
		event->ignore();
		break;
	}
}

bool Multiaxis::eventFilter(QObject *obj, QEvent *event)
{
	// Copied from single-axis and edited a bit
	if (event->type() == QEvent::WindowActivate) {
		for (int i=0;i<devcount;i++){
			if (settingsDlgs.at(i)->isVisible())
				settingsDlgs.at(i)->raise();
		}
		if (scriptDlg->isVisible())
			scriptDlg->raise();
		this->raise();
		this->activateWindow();
	}
	enum direction { decr, incr };
	int devnum = -1;
	int direction = -1;

	if (event->type() != QEvent::MouseButtonPress && event->type() != QEvent::MouseButtonRelease && event->type() != QEvent::MouseButtonDblClick) // speeds things up a bit
			return QWidget::eventFilter(obj, event);

	int saved_i = -1;
	for (int i=0; i<devcount; i++)
		if (obj == joyDecs.at(i)) {
			devnum = translate(i);
			saved_i = i;
			direction = decr;
			break;
		} else if (obj == joyIncs.at(i)) {
			devnum = translate(i);
			saved_i = i;
			direction = incr;
			break;
		}
	if (devnum < 0 || devnum > max_devices || (direction != decr && direction != incr) ) // If no match found, then skip the event
		return QWidget::eventFilter(obj, event);

	switch( event->type() ){
		case QEvent::MouseButtonDblClick:
		case QEvent::MouseButtonPress:
			if (direction == decr)
				devinterfaces.at(devnum)->command_left();
			else if (direction == incr)
				devinterfaces.at(devnum)->command_right();
			break;
		case QEvent::MouseButtonRelease:
			devinterfaces.at(devnum)->command_sstp();
			break;
		default:
			break;
	}
	if (saved_i == 0 || saved_i == 1)
		tgtXY->hide();
	else if (saved_i == 2)
		tgtZ->hide();

	// pass the event on to the parent class
	return QWidget::eventFilter(obj, event);
}

void Multiaxis::OnPickerXYPointSelected(const QPointF & point) {
	int coords[max_devices], ucoords[max_devices];
	double x, y, k;
	int i;
	x = point.x();
	y = point.y();
	tgtXY->setSamples(&x, &y, 1);
	tgtXY->show();
	i = translate(0);
	if (settingsDlgs.at(i)->uuStgs->enable) {
		k = settingsDlgs.at(i)->uuStgs->getUnitPerStep();
		x = x/k;
	}
	coords[i] = x;
	ucoords[i] = settingsDlgs.at(i)->motorStgs->getStepFrac()*(x - (int)x);
	i = translate(1);
	if (settingsDlgs.at(i)->uuStgs->enable) {
		k = settingsDlgs.at(i)->uuStgs->getUnitPerStep();
		y = y/k;
	}
	coords[i] = y;
	ucoords[i] = settingsDlgs.at(i)->motorStgs->getStepFrac()*(y - (int)y);
	for (int e=0; e<2; e++) {
		int i = translate(e);
		devinterfaces.at(i)->command_move(coords[i], ucoords[i]);
	}
}

void Multiaxis::OnPickerZPointSelected(const QPointF & point) {
	int coords[max_devices], ucoords[max_devices];
	double z, k;
	int i;
	z = point.y();

	double w[2], t[2];
	w[0] = -0.5; w[1] = 0.5;
	t[0] = t[1] = z;
	if (devcount > 2)
		tgtZ->setSamples(&w[0], &t[0], 2);

	i = translate(2);
	if (settingsDlgs.at(i)->uuStgs->enable) {
		k = settingsDlgs.at(i)->uuStgs->getUnitPerStep();
		z = z/k;
	}
	coords[i] = z;
	ucoords[i] = settingsDlgs.at(i)->motorStgs->getStepFrac()*(z - (int)z);
	devinterfaces.at(i)->command_move(coords[i], ucoords[i]);
}

void Multiaxis::SaveAxisConfig()
{
	XSettings settings(MakeProgramConfigFilename(), QSettings::IniFormat, QIODevice::ReadWrite);
	QStringList serials = getSerials();
	QString sn_str = getSerialsSortedConcat();

	settings.beginGroup("Multiaxis");
	settings.setValue("last_group", sn_str);
	settings.endGroup();

	settings.beginGroup(sn_str);
	for (int i=0; i<serials.length(); i++) {
		settings.setValue(toStr(i), serials.at(i));
	}
	this->FromUiToSettings(&settings);
	settings.endGroup();
}

void Multiaxis::LoadSingleConfigs()
{
	QStringList serials = getSerials();
	XSettings default_stgs(DefaultConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
	for (int i=0; i<serials.length(); i++) {
		XSettings settings(settingsDlgs.at(i)->MakeConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
		settingsDlgs.at(i)->AllPagesFromSettingsToUi(&settings, &default_stgs);

		((PageSliderSetupWgt*)settingsDlgs.at(i)->pageWgtsLst[PageSliderSetupNum])->FromUiToClass();
		((PageUserUnitsWgt*)settingsDlgs.at(i)->pageWgtsLst[PageUserUnitsNum])->FromUiToClass();
	}

	XSettings settings(MakeProgramConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
	settings.beginGroup(getSerialsSortedConcat());
	line_erase_delay = settings.value("line_erase_delay", 0).toDouble();
	settings.endGroup();
}

void Multiaxis::LoadLineEraseDelay()
{
	XSettings settings(MakeProgramConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
	settings.beginGroup(getSerialsSortedConcat());
	line_erase_delay = settings.value("line_erase_delay", 0).toDouble();
	settings.endGroup();
}

void Multiaxis::LoadSingleConfigs1()
{
	QStringList serials = getSerials();
	XSettings default_stgs(DefaultConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
	for (int i = 0; i<serials.length(); i++) {
		XSettings settings(settingsDlgs.at(i)->MakeConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
		settingsDlgs.at(i)->AllPagesFromSettingsToUi(&settings, &default_stgs);

		((PageSliderSetupWgt*)settingsDlgs.at(i)->pageWgtsLst[PageSliderSetupNum])->FromUiToClass();
		((PageUserUnitsWgt*)settingsDlgs.at(i)->pageWgtsLst[PageUserUnitsNum])->FromUiToClass();
	}

}

void Multiaxis::SaveSingleConfigs()
{
	QStringList serials = getSerials();
	for (int i=0; i<serials.length(); i++) {
		XSettings settings(settingsDlgs.at(i)->MakeConfigFilename(), QSettings::IniFormat, QIODevice::ReadWrite);
		settingsDlgs.at(i)->AllPagesFromUiToSettings(&settings);
	}

	XSettings settings(MakeProgramConfigFilename(), QSettings::IniFormat, QIODevice::ReadWrite);
	settings.beginGroup(getSerialsSortedConcat());
	settings.setValue("line_erase_delay", line_erase_delay);
	settings.endGroup();
}

QStringList Multiaxis::getSerials()
{
	QStringList serials;
	for (int e=0; e<devcount; e++) {
		int i = translate(e);
		serials.append(toStr(settingsDlgs.at(i)->controllerStgs->serialnumber));
	}
	return serials;
}

QString Multiaxis::getSerialsSortedConcat()
{
	QStringList serials = getSerials();

	QString sn_str;
	QList<uint32_t> sorted_serials;
	for (int i=0; i<serials.length(); i++)
		sorted_serials.append(serials.at(i).toUInt());
	qSort(sorted_serials);
	sn_str.append("SN_");
	for (int i=0; i<sorted_serials.length(); i++) {
		sn_str.append( toStr(sorted_serials.at(i)) + QString("-"));
	}
	sn_str.chop(1);
	return sn_str;
}

void Multiaxis::receiveSerials(QStringList serials, double delay)
{
	// Needed because some devices might be disabled when serials change
	for (int e=0; e<devcount; e++) {
		groupBoxs.at(e)->setDisabled(false);
		curPoss.at(e)->setDisabled(false);
		joyDecs.at(e)->setDisabled(false);
		joyIncs.at(e)->setDisabled(false);
//		moveTos.at(e)->setDisabled(false);
//		shiftOns.at(e)->setDisabled(false);
		QPen pen = mark.at(e)->linePen();
		pen.setStyle(Qt::SolidLine);
		mark.at(e)->setLinePen(pen);
		noDevice[e] = false;
	}

	int tr_new[max_devices];
	for(int i=0; i<serials.length(); i++) {
		uint32_t sn = serials.at(i).toUInt();
		for (int j=0; j<devcount;j++) {
			if (settingsDlgs.at(j)->controllerStgs->serialnumber == sn) {
				tr_new[i] = j;
			}
		}
	}
	for (int i=0; i<devcount; i++) {
		tr_matrix[i] = tr_new[i];
	}

	// This part will shuffle moveTo and shiftOn widgets into new positions because they are updated by internal serial and the order has changed
	ui.groupBox_Position->setUpdatesEnabled(false);
	for (int e=0; e<devcount; e++) {
		moveTos.at(e)->hide();
		shiftOns.at(e)->hide();
		ui.groupBox_Position->layout()->removeWidget(moveTos.at(e));
		ui.groupBox_Position->layout()->removeWidget(shiftOns.at(e));
	}
	for (int e=0; e<devcount; e++) {
		int i = translate(e);
		((QGridLayout*)ui.groupBox_Position->layout())->addWidget(moveTos.at(i), e+2, 1, 1, 1);
		((QGridLayout*)ui.groupBox_Position->layout())->addWidget(shiftOns.at(i),e+2, 2, 1, 1);
		moveTos.at(e)->show();
		shiftOns.at(e)->show();
	}
	ui.groupBox_Position->setUpdatesEnabled(true);

	rescalePlots();

	pagelog->FromUiToClass();
	line_erase_delay = delay;
}

void Multiaxis::rescalePlots()
{
	// Invalidate the borders and let main window UpdateState() redraw them
	for (int e=0; e<devcount; e++) {
		LeftBorder[e] = 0;
		RightBorder[e] = 0;
	}
}

int Multiaxis::translate(int num)
{
	return tr_matrix[num];
}

void Multiaxis::LoadConfigs()
{
	XSettings settings(MakeProgramConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
	XSettings default_stgs(DefaultConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);

#ifdef SERVICEMODE
	settings.beginGroup("Service_params");
	for (int i=0; i<settingsDlgs.length(); i++) {
		((PageStep3Wgt*)settingsDlgs.at(i)->pageWgtsLst[PageKeyNum])->SetKey(settings.value("Key", "0").toString());
	}
	settings.endGroup();
#endif

	settings.beginGroup(getSerialsSortedConcat());
	this->FromSettingsToUi(&settings);
	scriptDlg->FromSettingsToUi(&settings, &default_stgs);
	logDlg->FromSettingsToUi(&settings, &default_stgs);
	pagelog->FromSettingsToUi(&settings, &default_stgs);
	pagelog->FromUiToClass();
	for (int i=0; i<settingsDlgs.length(); i++) {
		XSettings settings(settingsDlgs.at(i)->MakeConfigFilename(), QSettings::IniFormat, QIODevice::ReadOnly);
		settingsDlgs.at(i)->FromSettingsToUi(&settings);
	}
	settings.endGroup();
}

void Multiaxis::SaveConfigs()
{
	XSettings settings(MakeProgramConfigFilename(), QSettings::IniFormat, QIODevice::ReadWrite);
	
	settings.beginGroup(getSerialsSortedConcat());
	this->FromUiToSettings(&settings);
	scriptDlg->FromUiToSettings(&settings);
	logDlg->FromUiToSettings(&settings);
	pagelog->FromUiToSettings(&settings);

	settings.endGroup();

	for (int i=0; i<settingsDlgs.length(); i++) {
		XSettings settings(settingsDlgs.at(i)->MakeConfigFilename(), QSettings::IniFormat, QIODevice::ReadWrite);
		settingsDlgs.at(i)->FromUiToSettings(&settings);
	}

}

void Multiaxis::FromUiToSettings(QSettings *settings)
{
	settings->beginGroup("multiWindow_params");
	if(!this->isMaximized()){
		settings->setValue("position", pos());
		settings->setValue("size", size());
	}
	settings->setValue("fullScreen", this->isMaximized());
	settings->endGroup();
}

void Multiaxis::FromSettingsToUi(QSettings *settings)
{
	int desktop_width = QApplication::desktop()->width();
	int desktop_height = QApplication::desktop()->height();
	QPoint point;
	QSize size;

	settings->beginGroup("multiWindow_params");
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