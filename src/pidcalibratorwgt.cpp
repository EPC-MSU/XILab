#include "pidcalibratorwgt.h"
#include "ui_pidcalibratorwgt.h"
#include <float.h>
#include <qwt_legend.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_canvas.h>

// Две скорости обновления данных (см. #18695) мсек
#define UPDATE_NORMAL_INTERVAL 20
#define UPDATE_FAST_INTERVAL   5

// Время измерения до\после остановки двигателя мсек
#define IDLE_INTERVAL 200

// Максимальная длина массива данных
#define MEASURES_MAX_LENGTH 25

static void calcDistance(int Speed, int Accel, int Decel, unsigned int CPT, unsigned int  feedback, int *Dist, int *Time);


PIDCalibratorWgt::PIDCalibratorWgt(DeviceInterface *_devinterface, uint32_t _serial, QWidget *parent) :
    QDialog(parent, Qt::Window),
	ui(new Ui::PIDCalibratorWgt),
	data_reader(_devinterface), zero_axis(QwtScaleDraw::BottomScale, 0.0)
{
    ui->setupUi(this);
	this->ui->KpBx->setRange(0, INT_MAX);
	this->ui->KiBx->setRange(0, INT_MAX);
	this->ui->KdBx->setRange(0, INT_MAX);
	this->ui->KpfBx->setRange(0, DBL_MAX);
	this->ui->KpfBx->setDecimals(4);
	this->ui->KifBx->setRange(0, DBL_MAX);
	this->ui->KifBx->setDecimals(4);
	this->ui->KdfBx->setRange(0, DBL_MAX);
	this->ui->KdfBx->setDecimals(4);

	this->ui->KpBx->installEventFilter(this);
	this->ui->KiBx->installEventFilter(this);
	this->ui->KdBx->installEventFilter(this);

	/* Настроить событие применения настроек по клавише ВВОД */
	this->return_press_eater = new ReturnPressEater(this->ui->applyBtn);
	
	this->ui->KpBx->installEventFilter(this->return_press_eater);
	this->ui->KiBx->installEventFilter(this->return_press_eater);
	this->ui->KdBx->installEventFilter(this->return_press_eater);
	this->ui->KpfBx->installEventFilter(this->return_press_eater);
	this->ui->KifBx->installEventFilter(this->return_press_eater);
	this->ui->KdfBx->installEventFilter(this->return_press_eater);
	this->ui->distanceBox->installEventFilter(this->return_press_eater);

	this->devinterface = _devinterface;

	ui->graphLayout->addWidget(&plot);

	/* Настройки отображения графика*/

	plot.setTitle("Oscilloscope"); // заголовок
	plot.setCanvasBackground(Qt::white); // цвет фона

	// Параметры осей координат

	plot.setAxisTitle(QwtPlot::yLeft, "Speed (counts per second)");
	
	plot.axisScaleEngine(QwtPlot::yRight)->setAttribute(QwtScaleEngine::Symmetric, true); // симметричное отн. 0 отображение для кривой ошибки

	plot.enableAxis(QwtPlot::yRight); // включить yRight ось
	plot.setAxisTitle(QwtPlot::yRight, "Error (counts)");

	plot.setAxisTitle(QwtPlot::xBottom, "Time (msec)");

	// Создаём легенду на канве

	plot.insertLegend(new QwtLegend(plot.canvas()), QwtPlot::ExternalLegend);
	QWidget *v = plot.legend();
	v->setGeometry(10, 10, v->width(), v->height());

	// Включить сетку

	this->grid.attach(&plot);

	// Добавить ось нулевой ошибки  по центру

	zero_axis.setAxes(QwtPlot::xBottom, QwtPlot::yRight);
	zero_axis.scaleDraw()->enableComponent(QwtAbstractScaleDraw::Ticks, false);
	zero_axis.scaleDraw()->enableComponent(QwtAbstractScaleDraw::Labels, false);
	zero_axis.scaleDraw()->setPenWidth(2);
	zero_axis.attach(&plot);

	// Создать кривую Speed

	this->speed_curve.setTitle("Speed");
	this->speed_curve.setPen(QPen(Qt::blue, 1)); // цвет и толщина кривой
	this->speed_curve.attach(&plot);

	// Создать кривую Error

	this->error_curve.setTitle("Error");
	this->error_curve.setPen(QPen(Qt::red, 1)); // цвет и толщина кривой
	this->error_curve.setYAxis(QwtPlot::yRight); // отображение по правой оси
	this->error_curve.attach(&plot);

	// Подключаем кнопки

	QObject::connect(ui->startBtn, SIGNAL(clicked()), this, SLOT(onStart()));
	QObject::connect(ui->stopBtn, SIGNAL(clicked()), this, SLOT(onStop()));
	QObject::connect(ui->applyBtn, SIGNAL(clicked()), this, SLOT(onApply()));

	// Подключаем поток

	QObject::connect(&(this->data_reader), SIGNAL(internalExit()), this, SLOT(onStop()), Qt::QueuedConnection);
	qRegisterMetaType<libximc::measurements_t>("measurements_t");
	QObject::connect(&(this->data_reader), SIGNAL(dataUpdated(measurements_t)), this, SLOT(dataUpdateSlot(measurements_t)), Qt::QueuedConnection);
	QObject::connect(&(this->data_reader), SIGNAL(dataReset()), this, SLOT(dataResetSlot()), Qt::QueuedConnection);

	// Обновляем PID
	this->loadSettings();

	// Настройки user units
	this->ui->distanceBox->setUnitType(UserUnitSettings::TYPE_COORD);
	this->ui->distanceBox->setFloatStyleAllowed(true);
	this->ui->distanceBox->setTempChangeAllowed(false);
	this->ui->distanceBox->setSerial(_serial);
}

void PIDCalibratorWgt::onStart()
{
	// Отключить кнопку start, включить stop
	this->ui->startBtn->setEnabled(false);
	this->ui->stopBtn->setEnabled(true);

	// Очистить окно
	this->onClear();

	// this->loadSettings();

	this->data_reader.startWork();
}

void PIDCalibratorWgt::onStop()
{
	// Отключить кнопку stop, включить start
	this->ui->startBtn->setEnabled(true);
	this->ui->stopBtn->setEnabled(false);

	this->data_reader.stopWork();
}

void PIDCalibratorWgt::onClear()
{
	this->points_error.clear();
	this->points_speed.clear();
	this->pointsCounter = 0;

	// ассоциировать набор точек с кривой
	this->speed_curve.setSamples(this->points_speed);
	this->error_curve.setSamples(this->points_error);

	// отрисовать
	this->plot.replot();
}

void PIDCalibratorWgt::onSetScale(int Scale)
{
	this->plot.setAxisAutoScale(QwtPlot::xBottom, false);
	this->plot.setAxisScale(QwtPlot::xBottom, 0, Scale);
}

void PIDCalibratorWgt::onApply()
{
	libximc::pid_settings_t pid_settings;
	this->devinterface->get_pid_settings(&pid_settings);

	pid_settings.Kpf = this->ui->KpfBx->value() / 1000.0;
	pid_settings.Kif = this->ui->KifBx->value() / 1000.0;
	pid_settings.Kdf = this->ui->KdfBx->value() / 1000.0;

	pid_settings.KpU = this->ui->KpBx->value();
	pid_settings.KiU = this->ui->KiBx->value();
	pid_settings.KdU = this->ui->KdBx->value();

	this->devinterface->set_pid_settings(&pid_settings);

	this->data_reader.setMoveSettings(this->ui->distanceBox->basevalueInt());

	emit changePIDSettings(pid_settings.Kpf, pid_settings.Kif, pid_settings.Kdf, pid_settings.KpU, pid_settings.KiU, pid_settings.KdU);
}

void PIDCalibratorWgt::loadSettings()
{
	// Загрузить настройки из контроллера, применить

	libximc::pid_settings_t pid_settings;
	libximc::entype_settings_t entype_settings;

	devinterface->get_pid_settings(&pid_settings);
	devinterface->get_entype_settings(&entype_settings);

	// Для DC мотора активны целочисленные поля
	this->ui->KpBx->setEnabled(entype_settings.EngineType == ENGINE_TYPE_DC);
	this->ui->KiBx->setEnabled(entype_settings.EngineType == ENGINE_TYPE_DC);
	this->ui->KdBx->setEnabled(entype_settings.EngineType == ENGINE_TYPE_DC);
	// Для BLDC - дробные поля
	this->ui->KpfBx->setEnabled((entype_settings.EngineType == ENGINE_TYPE_BRUSHLESS) || (entype_settings.EngineType == ENGINE_TYPE_STEP));
	this->ui->KifBx->setEnabled((entype_settings.EngineType == ENGINE_TYPE_BRUSHLESS) || (entype_settings.EngineType == ENGINE_TYPE_STEP));
	this->ui->KdfBx->setEnabled((entype_settings.EngineType == ENGINE_TYPE_BRUSHLESS) || (entype_settings.EngineType == ENGINE_TYPE_STEP));

	this->ui->KpBx->setValue(pid_settings.KpU);
	this->ui->KiBx->setValue(pid_settings.KiU);
	this->ui->KdBx->setValue(pid_settings.KdU);
	this->ui->KpfBx->setValue(pid_settings.Kpf * 1000.0);
	this->ui->KifBx->setValue(pid_settings.Kif * 1000.0);
	this->ui->KdfBx->setValue(pid_settings.Kdf * 1000.0);

	// Настройки движения

	libximc::move_settings_t move_settings;
	libximc::feedback_settings_t feedback_settings;

	this->devinterface->get_move_settings(&move_settings);
	this->devinterface->get_feedback_settings(&feedback_settings);

	// Настроить расстояние для движения
	int deltaPosition = 0;
	int time = 0;
	calcDistance(move_settings.Speed, move_settings.Accel, move_settings.Decel, feedback_settings.CountsPerTurn, feedback_settings.FeedbackType, &deltaPosition, &time);
	this->data_reader.setMoveSettings(deltaPosition);

	// Настроить примерную ширину графика по оси времени
	onSetScale(time + 2 * IDLE_INTERVAL);

	// Вывести расстояние
	this->ui->distanceBox->setBaseValue(deltaPosition);
}

PIDCalibratorWgt::~PIDCalibratorWgt()
{
	this->onStop();

	delete(this->plot.legend());
	delete(this->return_press_eater);

    delete ui;
}

void PIDCalibratorWgt::closeEvent(QCloseEvent *event)
{
	Q_UNUSED(event)
	this->onStop();
}

void PIDCalibratorWgt::showEvent(QShowEvent *event)
{
	Q_UNUSED(event)
	this->loadSettings();
}

void PIDCalibratorWgt::dataUpdateSlot(measurements_t measurements)
{
	for (unsigned int i = 0; i < measurements.Length; i++)
	{
		if (this->points_speed.size() <= (this->pointsCounter)) // добавили новую точку на график
		{
			this->points_error << QPointF(this->pointsCounter, measurements.Error[i]);
			this->points_speed << QPointF(this->pointsCounter, measurements.Speed[i]);
		}
		else // точка уже есть, нужно переписать её
		{
			this->points_error[pointsCounter].setY(measurements.Error[i]);
			this->points_error[pointsCounter].setX(pointsCounter);
			this->points_speed[pointsCounter].setX(pointsCounter);
			this->points_speed[pointsCounter].setY(measurements.Speed[i]);
		}

		this->pointsCounter++;
	}

	// ассоциировать набор точек с кривой
	this->speed_curve.setSamples(this->points_speed); 
	this->error_curve.setSamples(this->points_error);

	// отрисовать
	this->plot.replot();
}

void PIDCalibratorWgt::dataResetSlot()
{
	// После предыдущего построения остались лишне точки справа от графика, удалить их
	if (this->points_error.size() > pointsCounter)
	{
		this->points_error.remove(pointsCounter, points_error.size() - pointsCounter);
		this->points_speed.remove(pointsCounter, points_speed.size() - pointsCounter);
	}

	this->pointsCounter = 0;

	// Один график уже точно нарисован, можно включить Autoscale 
	this->plot.setAxisAutoScale(QwtPlot::xBottom, true);
}

void PIDCalibratorWgt::validate(void)
{

	if (ui->KpBx->value() > USHRT_MAX)
		ui->KpBx->setValue(USHRT_MAX);
	if (ui->KiBx->value() > USHRT_MAX)
		ui->KiBx->setValue(USHRT_MAX);
	if (ui->KdBx->value() > USHRT_MAX)
		ui->KdBx->setValue(USHRT_MAX);
}

bool  PIDCalibratorWgt::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() == QEvent::FocusOut)
	{
		if (object == ui->KpBx || object == ui->KiBx || object == ui->KdBx)
		{
			validate();
		}
	}

	return false;
}

/* 
 * Worker - класс - поток для считывания данных с устройства и обновления движения
 */

Worker::Worker(DeviceInterface* _devinterface) : do_work(0), devinterface(_devinterface)
{
	// Обработка всех событий\сигналов в этом же потоке
	this->moveToThread(this);
}

void Worker::startWork()
{
	this->do_work = true;
	this->move_state = MoveState::START;

	// Начинаем считывать данные
	this->devinterface->command_start_measurements();
	
	// Включаем поток обновления данных	
	if (!(this->isRunning()))
		this->start(QThread::HighPriority);
}

void Worker::stopWork()
{
	this->devinterface->command_stop();

	this->do_work = false;
	this->move_state = MoveState::FINISH;
}

void Worker::setMoveSettings(int _delta_position)
{
	this->delta_position = _delta_position;
}

void Worker::run()
{
	while (this->do_work)
	{
		static unsigned int time = 0;

		measurements_t measurements;
		if (this->devinterface->get_measurements(&measurements) != result_ok)
		{
			// прервать работу при ошибке
			time = 0;
			this->stopWork();
			emit internalExit();
			continue;
		}

		// отправить данные
		emit dataUpdated(measurements);

		bool status_move = (this->devinterface->cs->status().MvCmdSts & MVCMD_RUNNING);

		if (status_move)
		{
			// Мы считаем время до и после двжиения: нет смысла увеличивать счётчик во время движения
			time = 0;
		}

		// Прошло время ожидания первого движения
		if ((time > IDLE_INTERVAL) && (this->move_state == MoveState::START))
		{
			// Запускаем движение вправо
			this->startMove(0);
			this->move_state = MoveState::MOVE;
			time = 0;
		}

		// Остановились, и прошло время, в течение которого ещё измеряем после  остановки
		if ((time > IDLE_INTERVAL) && (this->move_state == MoveState::MOVE) && !(status_move))
		{
			this->move_state = MoveState::FINISH;

			// Вернуться 
			this->startMove(1);

			// (подождать, пока вернётся ... )
			this->devinterface->command_wait_for_stop(50);

			// И начать заново! 
			time = 0;
			this->move_state = MoveState::START;
			emit dataReset();
			this->devinterface->command_start_measurements();

			continue;
		}

		// Увеличить частоту опроса при переполнении массива
		unsigned int sleep_time = UPDATE_NORMAL_INTERVAL;
		if (measurements.Length == MEASURES_MAX_LENGTH)
			sleep_time = UPDATE_FAST_INTERVAL;

		time += sleep_time;
		this->msleep(sleep_time);
	}
}

static void calcDistance(int Speed, int Accel, int Decel, unsigned int CPT, unsigned int  feedback, int *Dist, int *Time)
{
	/*
	* Считает расстояние, которое нужно пройти по трапеции.
	* Скорость - rpm, Ускорение, Замедление - rpm/s
	* Расстояние (выход) - пульсы энкодера
	* Время (выход) - время движения
	* (предполагаем, что используется только для двигателя с обратной связью)
	* См. формулы в #18695
	*/

	if ((Accel == 0) || (Decel == 0) || (Speed == 0) || (CPT == 0))
	{
		return;
	}

	float fSpeed = (float)Speed;// *(float)CPT / 60.0F; // 1 rpm = 1/60 rps = CPT/60 PPS
	float fAccel = (float)Accel;// *(float)CPT / 60.0F;
	float fDecel = (float)Decel;// *(float)CPT / 60.0F;

	// Since at this stage the PID is present in both encoder and non-encoder modes, 
	// it is necessary to recalculate it.
	if (feedback == FEEDBACK_ENCODER || feedback == FEEDBACK_ENCODER_MEDIATED)
	{
		fSpeed *= (float)CPT / 60.0F; // 1 rpm = 1/60 rps = CPT/60 PPS
		fAccel *= (float)CPT / 60.0F;
		fDecel *= (float)CPT / 60.0F;
	}

	// For large values, this is a linear motor with a linear encoder. 
	// In them, the acceleration values in the profile are reduced by 4 times. 
	// You need to restore it to normal.
	if (feedback == FEEDBACK_ENCODER )
	if (CPT > 50000)
	{
		fAccel *= (float)4;
		fDecel *= (float)4;
	}

	float S0 = fSpeed * fSpeed / 2 / fAccel;
	float S2 = fSpeed * fSpeed / 2 / fDecel;

	float S1 = (S0 + S2) * 6;

	int S = S0 + S1 + S2;

	int T = 1000.0F * (sqrtf(2.0F * S0 / fAccel) + sqrtf(2.0F * S2 / fDecel) + (S1 / fSpeed));

	*Time = T;
	*Dist = S;
}

void Worker::startMove(bool left) // если left == True, то влево, иначе - вправо
{
	// Начать движение

	int delta_position = this->delta_position * (left ? -1 : 1);

	this->devinterface->command_movr(delta_position, 0);
}