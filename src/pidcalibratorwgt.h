#ifndef PID_CALIBRATOR_H
#define PID_CALIBRATOR_H

#include <QDialog>

#include <qevent.h>
#include <qpushbutton.h>

#include <QAtomicInt>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_scaleitem.h>

#include <qthread.h>
#include <qtimer.h>

#include "deviceinterface.h"

namespace Ui {
	class PIDCalibratorWgt;
}

namespace MoveState {
	enum {START, MOVE, FINISH};
}

/*
 * ReturnPressEater - класс - обработчик нажатия Return на жлементах настройки
 */

class ReturnPressEater : public QObject
{
	Q_OBJECT

private:
	QPushButton *button;

protected:
	bool eventFilter(QObject *obj, QEvent *event)
	{
		if (event->type() == QEvent::KeyPress)
		{
			int key = static_cast<QKeyEvent *>(event)->key();

			if (key == Qt::Key_Enter || key == Qt::Key_Return)
			{
				this->button->click();
				return true;
			}
		}
		return QObject::eventFilter(obj, event);
	}

public:
	ReturnPressEater(QPushButton* btn)
	{
		this->button = btn;
	}
};


/*
 * Worker - класс - поток для считывания данных с устройства и обновления движения
 */

class Worker : public QThread
{
	Q_OBJECT
	void run();

private:
	QAtomicInt do_work;
	QAtomicInt move_state;
	DeviceInterface* devinterface;

	int delta_position;

	void startMove(bool left);

public:
	explicit Worker(DeviceInterface* _devinterface);

public slots:
	void startWork();
	void stopWork();
	void setMoveSettings(int _delta_position);

signals:
	void dataUpdated(measurements_t measurements); // добавляется новая точка
	void dataReset(); // обновляется график
	void internalExit();  // поток завершился по внутренним причинам
};


/*
 * PIDCalibratorWgt - виджет - осциллоскоп
 */

class PIDCalibratorWgt : public QDialog
{
    Q_OBJECT

public:
	explicit PIDCalibratorWgt(DeviceInterface* _devinterface, uint32_t _serial, QWidget *parent = 0);
    ~PIDCalibratorWgt();

	virtual bool eventFilter(QObject *object, QEvent *e);

private:
	Ui::PIDCalibratorWgt *ui;
	Worker data_reader;

	QwtPlot plot;
	QwtPlotGrid grid;
	QwtPlotScaleItem zero_axis;
	int pointsCounter = 0;

	QwtPlotCurve speed_curve;
	QwtPlotCurve error_curve;

	QPolygonF points_speed;
	QPolygonF points_error;

	ReturnPressEater *return_press_eater;

	DeviceInterface *devinterface;

	void closeEvent(QCloseEvent *event);
	void showEvent(QShowEvent* event);

	void loadSettings(); // заполнить ПИД коэффициенты
	void validate(void);

private slots:
	void onStart();
	void onStop();
	void onClear(); // очистка графика
	void onApply(); // применить новые PID коэффициенты
	void onSetScale(int Scale); // сообщить графику масштаб по X

	void dataUpdateSlot(measurements_t measurements);
	void dataResetSlot();

signals:
	void changePIDSettings(float Kpf, float Kif, float Kdf, int Kp, int Ki, int Kd);
};

#endif // PID_CALIBRATOR_H
