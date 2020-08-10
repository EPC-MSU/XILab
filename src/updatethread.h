#ifndef UPDATETHREAD_H
#define UPDATETHREAD_H

#include <QThread>
#include <QTimer>
#include <QDateTime>
#include <deviceinterface.h>
#include <userunitsettings.h>
#include <status.h>
#include <basethread.h>

extern const int UPDATE_INTERVAL;

class UpdateThread: public BaseThread
{
	Q_OBJECT
public:
	UpdateThread(int _number, QObject *parent, char* dev_name, DeviceInterface *_devinterface);
	~UpdateThread();
	
	bool waitForReconnect(int ms = 0);

    char device_name[255];
	QString QSdev_name;

	DeviceInterface *devinterface;
private:
	result_t result;
	int reconnect_time;
	status_t status;
	chart_data_t chart_data;
	analog_data_t analog_data;
	int number;
	Cactus *cs;
	QDateTime last_wfr_datetime; // "wfr" stands for waitForReconnect

public slots:
	void setReconnectInterval(int timeMs);

private slots:
	void user_update();

signals:
	void done();
};

#endif // UPDATETHREAD_H
