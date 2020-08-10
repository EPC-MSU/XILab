#include <status.h>

Cactus::Cactus()
{
	updates_enabled = true;
	analog_enabled = false;
	charts_enabled = false;
	additional_enabled = false;
	use_calibration = false;
}

Cactus::~Cactus()
{
}

// Setters
void Cactus::setUpdatesEnabled(bool flag)
{
	QMutexLocker locker(&mutex);
	this->updates_enabled = flag;
}
void Cactus::setAnalogEnabled(bool flag)
{
	QMutexLocker locker(&mutex);
	this->analog_enabled = flag;
}
void Cactus::setChartsEnabled(bool flag)
{
	QMutexLocker locker(&mutex);
	this->charts_enabled = flag;
}
void Cactus::setAdditionalEnabled(bool flag)
{
	QMutexLocker locker(&mutex);
	this->additional_enabled = flag;
}
void Cactus::setUseCalibration(bool flag)
{
	QMutexLocker locker(&mutex);
	this->use_calibration = flag;
}
/*	
void Cactus::setMode(int device_mode) // wtf, int?
{
	QMutexLocker locker(&mutex);
	this->device_mode = device_mode;
}
*/
void Cactus::setStatusData(status_t status)
{
	QMutexLocker locker(&mutex);
	this->_status = status;
	this->last_update_time = QDateTime::currentDateTimeUtc();
}

void Cactus::setStatusCalbData(status_calb_t status_calb)
{
	QMutexLocker locker(&mutex);
	this->status_calb = status_calb;
	this->last_update_time = QDateTime::currentDateTimeUtc();
	// Copy fields because mainwindow expects most data in status() even in calb-mode
	_status.MoveSts = status_calb.MoveSts;
	_status.MvCmdSts = status_calb.MvCmdSts;
	_status.PWRSts = status_calb.PWRSts;
	_status.EncSts = status_calb.EncSts;
	_status.WindSts = status_calb.WindSts;
	//int CurPosition;
	//int uCurPosition;
	_status.EncPosition = status_calb.EncPosition;
	//int CurSpeed;
	//int uCurSpeed;
	_status.Ipwr = status_calb.Ipwr;
	_status.Upwr = status_calb.Upwr;
	_status.Iusb = status_calb.Iusb;
	_status.Uusb = status_calb.Uusb;
	_status.CurT = status_calb.CurT;
	_status.Flags = status_calb.Flags;
	_status.GPIOFlags = status_calb.GPIOFlags;
	_status.CmdBufFreeSpace = status_calb.CmdBufFreeSpace;
}

void Cactus::setChartData(chart_data_t chart_data)
{
	QMutexLocker locker(&mutex);
	this->chart_data = chart_data;
}

void Cactus::setAnalogData(analog_data_t analog_data)
{
	QMutexLocker locker(&mutex);
	this->analog_data = analog_data;
}

void Cactus::setCalibrationData(calibration_t calb_data)
{
	QMutexLocker locker(&mutex);
	this->calb_data = calb_data;
}

void Cactus::setLastUpdateTime(QDateTime time)
{
	QMutexLocker locker(&mutex);
	this->last_update_time = time;
}

void Cactus::setConnect(bool connect)
{
	QMutexLocker locker(&mutex);
	this->_connect = connect;
}

void Cactus::updateCalibration(UserUnitSettings uus, unsigned int MicrostepMode)
{
	QMutexLocker locker(&mutex);
	this->use_calibration = uus.enable;
	calb_data.A = uus.getUnitPerStep();
	calb_data.MicrostepMode = MicrostepMode;
}
