#ifndef STATUS_H
#define STATUS_H

#include <QMutex>
#include <QDateTime>
#include "userunitsettings.h"

namespace libximc
{
#include <ximc.h>
}
using namespace libximc;

class Cactus : public QObject
{
	Q_OBJECT
public:
	Cactus(void);
	~Cactus(void);

	// Getters
	bool updatesEnabled()		{ return updates_enabled; }
	bool analogEnabled()		{ return analog_enabled; }
	bool chartsEnabled()		{ return charts_enabled; }
	bool additionalEnabled()	{ return additional_enabled; }
	bool useCalibration()		{ return use_calibration; }

	status_t status()			{ return _status; }
	status_calb_t statusCalb()	{ return status_calb; }
	chart_data_t chartData()	{ return chart_data; }
	analog_data_t analogData()	{ return analog_data; }
	calibration_t calbData()	{ return calb_data; }
	bool connect()				{ return _connect; }
	QDateTime lastUpdateTime()	{ return last_update_time; }

	// Setters
	void setUpdatesEnabled(bool flag);
	void setAnalogEnabled(bool flag);
	void setChartsEnabled(bool flag);
	void setAdditionalEnabled(bool flag);
	void setUseCalibration(bool flag);

//	void setMode(int device_mode); /// external mode setting is forbidden
	void setStatusData(status_t state);
	void setStatusCalbData(status_calb_t status_calb);
	void setChartData(chart_data_t chart_data);
	void setAnalogData(analog_data_t analog_data);
	void setCalibrationData(calibration_t calb_data);
	void setLastUpdateTime(QDateTime time); // are we sure we want to do it from outside?

	void setConnect(bool connect);

public slots:
	void updateCalibration(UserUnitSettings uustgs, unsigned int MicrostepMode);

private:
	QMutex mutex;

	bool updates_enabled;
	bool analog_enabled;
	bool charts_enabled;
	bool additional_enabled;
	bool use_calibration;

	status_t _status;
	status_calb_t status_calb;
	chart_data_t chart_data;
	analog_data_t analog_data;
	calibration_t calb_data;
	bool _connect;

	QDateTime last_update_time;
};

#endif //STATUS_H
